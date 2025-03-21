/**
 * @file main.c
 * @brief Main entry point for the server application.
 *
 * This program initializes a server socket, parses a configuration file,
 * and starts listening for incoming connections. It uses a reactive server
 * model to handle client requests.
 *
 * @details
 * The program expects a single command-line argument specifying the path
 * to the configuration file. The configuration file must contain a "PORT"
 * key specifying the port number on which the server will listen.
 *
 * The program performs the following steps:
 * 1. Parses the configuration file to extract server settings.
 * 2. Initializes a server socket using the specified port.
 * 3. Sets up the request handler.
 * 4. Starts listening for incoming connections.
 * 5. Cleans up resources and threads upon termination.
 *
 * If any step fails, the program exits with an error code.
 *
 * @param argc The number of command-line arguments.
 * @param argv The array of command-line arguments.
 *             argv[1] should contain the path to the configuration file.
 *
 * @return Returns 0 on successful execution, or a non-zero value on failure.
 */
#include <stdio.h>
#include <stdlib.h>
#include "server/reactive.h"

int main(int argc, char *argv[]) {
    S_socket *socket;
    Dict *conf;
    int status;
    char *port;
    int int_port;

    if (argc != 2) {
        printf("Escribe la dirección del archivo de configuración\n");
        return 1;
    }

    conf = conf_parse(argv[1]);
    if (conf == NULL) {
        exit(-1);
    }

    port = get_value(conf, "PORT");
    int_port = atoi(port);
    if (int_port == 0) {
        free_dict(conf);
        exit(-1);
    }

    socket = init_socket(int_port);
    if (!socket) {
        printf("Error al inicializar el socket\n");
        free_dict(conf);
        exit(-1);
    }

    printf("Socket inicializado %d\n", socket->socket);

    init_handler();

    status = server_listen(socket, conf);

    cleanup_threads();

    if (status != 0) {
        free_socket(socket);
        exit(-1);
    }

    return 0;
}
