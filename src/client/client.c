/**
 * @file client.c
 * @brief Client application for communicating with a server.
 *
 * This program implements a client that connects to a server using TCP sockets.
 * It allows the user to send HTTP-like requests (GET, POST, OPTIONS) and receive
 * responses from the server. The client also supports saving binary responses
 * (e.g., images, videos) to files.
 * 
 * @author Miguel Paterson & Mijaíl Sazhín
 * @date 03-2025
 *
 * @details
 * ## Features:
 * - Connects to a server using a specified IP address and port.
 * - Sends HTTP-like requests (GET, POST, OPTIONS).
 * - Handles binary responses and saves them to files.
 * - Allows the user to close the connection or exit the program.
 *
 * ## Usage:
 * Compile the program and run it with the following arguments:
 * ```
 * ./client [server_ip] [server_port]
 * ```
 * - `server_ip`: The IP address of the server (default: 127.0.0.1).
 * - `server_port`: The port number of the server (default: 8080).
 *
 * ## Example:
 * ```
 * ./client 192.168.1.1 8080
 * ```
 * This connects the client to the server at IP `192.168.1.1` on port `8080`.
 *
 * ## Notes:
 * - The program creates a `downloads` directory in the current working directory
 *   to save binary files.
 * - The user can exit the program by typing `EXIT` or close the connection with
 *   the server by typing `CLOSE`.
 *
 * ## Error Handling:
 * - The program handles errors such as socket creation failure, connection failure,
 *   and file I/O errors, and provides appropriate error messages.
 *
 * @param argc The number of command-line arguments.
 * @param argv The array of command-line arguments.
 *             argv[1] should contain the IP of the server.
 *             argv[2] should contain the PORT of the server.
 *
 * @return Returns 0 on successful execution, or a non-zero value if an error occurs.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/stat.h>

#define BUFFER_SIZE 4096

/**
 * @brief Saves a binary response received from a socket into a file.
 *
 * This function reads binary data from a socket and writes it to a file. It
 * starts by writing the initial data provided, followed by reading the
 * remaining data from the socket until the specified content length is reached.
 *
 * @param sock The socket file descriptor from which the binary data is received.
 * @param filename The name of the file where the binary data will be saved.
 * @param initial_data A pointer to the initial chunk of data to be written to the file.
 * @param initial_size The size of the initial data in bytes.
 * @param content_length The total size of the binary content to be saved, including the initial data.
 */
void save_binary_response(int sock, const char *filename, const char *initial_data, int initial_size, int content_length) {
    char buffer[BUFFER_SIZE];
    ssize_t bytes_received;
    int total_received = initial_size;

    mkdir("downloads", 0777);

    char filepath[512];
    snprintf(filepath, sizeof(filepath), "downloads/%s", filename);

    FILE *file = fopen(filepath, "wb");
    if (!file) {
        perror("Error al abrir el archivo para guardar la respuesta");
        return;
    }

    printf("Guardando archivo en: %s\n", filepath);

    if (initial_size > 0) {
        fwrite(initial_data, 1, initial_size, file);
    }

    while (total_received < content_length) {
        bytes_received = recv(sock, buffer, BUFFER_SIZE, 0);
        if (bytes_received <= 0) {
            break;
        }
        fwrite(buffer, 1, bytes_received, file);
        total_received += bytes_received;
    }

    fclose(file);
    printf("Archivo binario guardado en '%s' (%d bytes recibidos).\n", filepath, total_received);
}

/**
 * @brief Sends a request to the server through the specified socket.
 *
 * This function is responsible for sending a request to the server
 * using the provided socket descriptor. The exact details of the
 * request (e.g., format, content) depend on the implementation
 * within the function.
 *
 * @param sock The socket file descriptor used to communicate with the server.
 * @return int Returns 0 on success, or a negative value on failure.
 */
int send_request(int sock) {
    char request[BUFFER_SIZE * 2];
    char method[10], path[256], data[BUFFER_SIZE];
    char buffer[BUFFER_SIZE];
    ssize_t bytes_received;
    int content_length = 0;
    char *body_start = NULL;
    int is_binary = 0;

    while (1) {
        printf("Ingrese el método (GET/POST/OPTIONS/CLOSE para cerrar conexión/EXIT para salir): ");
        scanf("%s", method);
        getchar();

        if (strcasecmp(method, "EXIT") == 0) {
            printf("Saliendo...\n");
            close(sock);
            exit(0);
        }

        if (strcasecmp(method, "CLOSE") == 0) {
            printf("Cerrando conexión con el servidor...\n");
            close(sock);
            return -2;
        }

        if (strcasecmp(method, "OPTIONS") != 0 && strcasecmp(method, "GET") && strcasecmp(method, "POST")) {
            printf("Método no aceptado\n");
            continue;
        }

        printf("Ingrese la ruta (ejemplo: / o /data): ");
        fgets(path, sizeof(path), stdin);
        path[strcspn(path, "\n")] = 0;

        char *filename = strrchr(path, '/');
        if (filename) {
            filename++;
        } else {
            filename = "archivo.bin";
        }

        if (strcasecmp(method, "POST") == 0) {
            printf("Ingrese los datos a enviar: ");
            fgets(data, sizeof(data), stdin);
            data[strcspn(data, "\n")] = 0;
            snprintf(request, sizeof(request), "%s %s HTTP/1.1\r\nHost: servidor\r\nContent-Length: %ld\r\n\r\n%s", method, path, strlen(data), data);
        } else {
            snprintf(request, sizeof(request), "%s %s HTTP/1.1\r\nHost: servidor\r\n\r\n", method, path);
        }

        if (send(sock, request, strlen(request), 0) < 0) {
            perror("Error al enviar la solicitud");
            close(sock);
            return -1;
        }

        printf("Respuesta del servidor:\n");

        memset(buffer, 0, sizeof(buffer));
        bytes_received = recv(sock, buffer, BUFFER_SIZE - 1, 0);
        if (bytes_received <= 0) {
            printf("\nEl servidor ha cerrado la conexión.\n");
            close(sock);
            return -1;
        }

        buffer[bytes_received] = '\0';
        printf("%s", buffer);

        char *content_length_str = strstr(buffer, "Content-Length:");
        if (content_length_str) {
            sscanf(content_length_str, "Content-Length: %d", &content_length);
        }

        if (strstr(buffer, "Content-Type: image") || 
            strstr(buffer, "Content-Type: application/octet-stream") ||
            strstr(buffer, "Content-Type: video/mp4") || 
            strstr(buffer, "Content-Type: video/mpeg")) {
            is_binary = 1;
        }

        body_start = strstr(buffer, "\r\n\r\n");
        if (body_start) {
            body_start += 4;
            int header_size = body_start - buffer;
            int body_bytes_received = bytes_received - header_size;

            if (is_binary) {
                printf("Recibiendo archivo binario...\n");
                save_binary_response(sock, filename, body_start, body_bytes_received, content_length);
            } else {
                printf("%s", body_start);
                int remaining = content_length - body_bytes_received;
                while (remaining > 0) {
                    bytes_received = recv(sock, buffer, BUFFER_SIZE - 1, 0);
                    if (bytes_received <= 0) break;
                    buffer[bytes_received] = '\0';
                    printf("%s", buffer);
                    remaining -= bytes_received;
                }
            }
        }

        printf("\n");
    }

    return 0;
}

int main(int argc, char *argv[]) {
    char server_ip[64] = "127.0.0.1";
    int server_port = 8080;
    int sock, st = 0;
    struct sockaddr_in server;
    struct hostent *host;
    char aux = 'n';

    if (argc > 1) {
        strncpy(server_ip, argv[1], sizeof(server_ip) - 1);
        server_ip[sizeof(server_ip) - 1] = '\0';
    }
    if (argc > 2) {
        server_port = atoi(argv[2]);
    }

    while (1) {
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock == -1) {
            perror("No se pudo crear el socket");
            continue;
        }

        server.sin_family = AF_INET;
        server.sin_port = htons(server_port);
        
        if ((host = gethostbyname(server_ip)) != NULL) {
            memcpy(&server.sin_addr, host->h_addr_list[0], host->h_length);
        } else {
            server.sin_addr.s_addr = inet_addr(server_ip);
        }

        if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
            perror("Error al conectar con el servidor");
            close(sock);
            continue;
        }

        printf("Conectado al servidor %s en el puerto %d.\n", server_ip, server_port);
        st = send_request(sock);
        if (st == -2) {
            printf("Se ha desconectado. ¿Quiere volver a establecer la conexión? Y/N\n");
            scanf(" %c", &aux);
            if (aux == 'n' || aux == 'N')
                break;
            else if (aux == 'y' || aux == 'Y')
                continue;
            else {
                printf("Valor no aceptado\n");
                return -1;
            }
        }
    }

    return 0;
}