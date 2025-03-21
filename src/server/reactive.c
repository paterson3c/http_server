/**
 * @file reactive.c
 * @brief Implementation of a reactive server handling multiple client connections using threads.
 *
 * This file contains the implementation of a reactive server that handles client connections
 * using threads. It supports HTTP request parsing, response generation, and file sending.
 * The server is designed to handle a configurable number of simultaneous clients and includes
 * mechanisms for timeout handling, graceful shutdown, and resource cleanup.
 *
 * @author Miguel Paterson & Mijaíl Sazhín
 * @date 03-2025
 */
#include "reactive.h"
#include <signal.h>
#include <sys/types.h>
#include <pthread.h>

/* ---------------------- Global objects ---------------------- */
S_socket *s_socket;
Dict *conf;

int active_clients = 0;
pthread_mutex_t active_clients_mutex = PTHREAD_MUTEX_INITIALIZER;

int client_sockets[MAX_THREADS];
pthread_mutex_t client_sockets_mutex = PTHREAD_MUTEX_INITIALIZER;
int max_clients = 0;

volatile sig_atomic_t shutdown_flag = 0;

int timeout = 10;

/* ---------------------- Private Functions ---------------------- */

/**
 * @brief Cleanup handler for client threads.
 *
 * This function is called when a client thread is terminated. It ensures that all resources
 * associated with the client connection are properly released, including the parser, response,
 * and socket. It also updates the global state to reflect the disconnection of the client.
 *
 * @param arg Pointer to an array containing the client socket, parser, and response.
 */
void _cleanup_handler(void *arg) {
    void **args = (void **)arg;
    int *client_socket = (int *)args[0];
    Parser **parser = (Parser **)args[1];
    Response **response = (Response **)args[2];

    if (*parser) {
        free_parser(*parser);
        *parser = NULL;
    }

    if (*response) {
        free_response(*response);
        *response = NULL;
    }

    printf("Cerrando conexión del cliente (socket %d)...\n", *client_socket);
    close(*client_socket);
    pthread_mutex_lock(&client_sockets_mutex);
    for (int i = 0; i < max_clients; i++) {
        if (client_sockets[i] == *client_socket) {
            client_sockets[i] = 0;
            break;
        }
    }
    pthread_mutex_unlock(&client_sockets_mutex);
    pthread_mutex_lock(&active_clients_mutex);
    active_clients--;
    pthread_mutex_unlock(&active_clients_mutex);
}

/**
 * @brief Signal handler for timeout events.
 *
 * This function is triggered when a timeout signal (SIGALRM) is received. It is used to handle
 * client connection timeouts.
 *
 * @param sig Signal number (unused).
 */
void timeout_handler(int sig) {
}

/**
 * @brief Signal handler for SIGINT (interrupt signal).
 *
 * This function is invoked when the process receives the SIGINT signal,
 * typically triggered by pressing Ctrl+C in the terminal. It allows the
 * program to handle the signal gracefully, such as performing cleanup
 * operations or shutting down the server safely.
 *
 * @param sig The signal number (expected to be SIGINT).
 */
void sigint_handler(int sig) {
    printf("\nSIGINT recibido, cerrando servidor...\n");
    shutdown_flag = 1;
    shutdown(s_socket->socket, SHUT_RDWR);
    close(s_socket->socket);

    pthread_mutex_lock(&client_sockets_mutex);
    for (int i = 0; i < max_clients; i++) {
        if (client_sockets[i] != 0) {
            shutdown(client_sockets[i], SHUT_RD);
            close(client_sockets[i]);
            client_sockets[i] = 0;
        }
    }
    pthread_mutex_unlock(&client_sockets_mutex);
}

/**
 * @brief Thread function to handle a single client connection.
 *
 * This function is executed by a thread to handle communication with a single client. It reads
 * HTTP requests, parses them, generates responses, and sends them back to the client. The thread
 * also handles connection timeouts and ensures proper cleanup of resources.
 *
 * @param arg Pointer to the client socket descriptor.
 * @return NULL
 */
void *_handle_client(void *arg) {
    int client_socket = *(int *)arg;
    free(arg);
    Parser *parser = NULL;
    Response *response = NULL;
    char buffer[BUFFER_SIZE], *postargs;
    ssize_t bffread;
    int keep_alive = 1;

    void *cleanup_args[3] = {&client_socket, &parser, &response};
    pthread_cleanup_push(_cleanup_handler, cleanup_args);

    struct sigaction sa;
    sa.sa_handler = timeout_handler;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGALRM, &sa, NULL);

    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGALRM);
    pthread_sigmask(SIG_UNBLOCK, &mask, NULL);

    while(keep_alive && !shutdown_flag) {
        memset(buffer, 0, BUFFER_SIZE);

        alarm(timeout);
        bffread = read(client_socket, buffer, BUFFER_SIZE - 1);
        alarm(0);
        if (bffread <= 0) {
            keep_alive = 0;
            continue;
        }

        printf("LEIDO:\n%s\n", buffer);

        parser = pars_http(buffer, conf);
        if (parser ==  NULL) {
            perror("http_parser");
            break;
        }

        if (parser->method == POST) {
            postargs = strstr(buffer, "\r\n\r\n");
            strcpy(parser->args, postargs);
            parser->args += 4;
        }

        response = create_response(parser);
        if (response == NULL) {
            perror("Response");
            break;
        }

        if(strstr(buffer, "Connection: close") != NULL || shutdown_flag || (parser->version == HTTP1_0)) {
            keep_alive = 0;
        }
        
        send(client_socket, response->header, strlen(response->header), MSG_NOSIGNAL);
        printf("Enviado header\n");

        if (parser->status == HTTP_OK && parser->method != OPTIONS) {
            send_file(client_socket, response);
            printf("Enviado archivo\n");
        }

        free_parser(parser);
        free_response(response);
        parser = NULL;
        response = NULL;
    }
    
    pthread_cleanup_pop(1);
    
    return NULL;
}

/* ---------------------- Public Functions ---------------------- */
int init_handler() {
    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = sigint_handler;

    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("Error al registrar SIGINT");
        return -1;
    }

    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGALRM);
    pthread_sigmask(SIG_BLOCK, &mask, NULL);

    signal(SIGPIPE, SIG_IGN);

    return 0;
}

void cleanup_threads() {
    printf("Esperando a que los hilos terminen...\n");
    while (1) {
        pthread_mutex_lock(&active_clients_mutex);
        if (active_clients == 0) {
            pthread_mutex_unlock(&active_clients_mutex);
            break;
        }
        pthread_mutex_unlock(&active_clients_mutex);
        sleep(1);
    }

    if (s_socket) {
        close(s_socket->socket);
        free_socket(s_socket);
    }

    if (conf) {
        free_dict(conf);
    }

    printf("Servidor cerrado correctamente.\n");
}

int server_listen(S_socket *e_s_socket, Dict *e_conf){
    socklen_t clilen;
    int *client_socket;
    pthread_t thread_id;
    max_clients = atoi(get_value(e_conf, "MAX_CLIENTS"));
    if (max_clients <= 0 || max_clients > MAX_THREADS) {
        perror("MAX_CLIENTS");
        return -1;
    }

    timeout = atoi(get_value(e_conf, "TIMEOUT"));
    if (timeout <= 0){
        printf("Timeout invalido");
        return -1;
    }

    s_socket = e_s_socket;
    conf = e_conf;
    clilen = sizeof(s_socket->address);

    printf("Servidor escuchando en el puerto %d...\n", atoi(get_value(conf, "PORT")));

    while (!shutdown_flag) {
        pthread_mutex_lock(&active_clients_mutex);
        if (active_clients >= max_clients) {
            pthread_mutex_unlock(&active_clients_mutex);
            printf("Se alcanzó el límite de clientes simultáneos (%d). Rechazando conexión.\n", max_clients);
            sleep(5);
            continue;
        }
        pthread_mutex_unlock(&active_clients_mutex);

        client_socket = malloc(sizeof(int));
        *client_socket = accept(s_socket->socket, (struct sockaddr*) &s_socket->address, &clilen);
        
        if (shutdown_flag) {
            free(client_socket);
            break;
        }

        pthread_mutex_lock(&client_sockets_mutex);
        for (int i = 0; i < max_clients; i++) {
            if (client_sockets[i] == 0) {
                client_sockets[i] = *client_socket;
                break;
            }
        }
        pthread_mutex_unlock(&client_sockets_mutex);

        if (*client_socket < 0) {
            perror("Accept");
            free(client_socket);
            continue;
        }

        printf("Nueva conexion aceptada \n");

        if (pthread_create(&thread_id, NULL, _handle_client, client_socket) != 0) {
            perror("Error en pthread_create");
            close(*client_socket);
            free(client_socket);
            continue;
        }

        pthread_mutex_lock(&active_clients_mutex);
        active_clients++;
        pthread_mutex_unlock(&active_clients_mutex);

        pthread_detach(thread_id);
    }
    
    return 0;
}
