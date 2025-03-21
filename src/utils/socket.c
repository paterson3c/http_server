/**
 * @file socket.c
 * @brief This file contains utility functions for managing sockets, including
 *        initialization, binding, listening, and cleanup of socket resources.
 *
 * The functions provided in this file are designed to simplify the process of
 * creating and managing sockets for network communication. It includes the
 * ability to initialize a socket, bind it to a specific port, listen for
 * incoming connections, and free the allocated resources when no longer needed.
 * 
 * @author Miguel Paterson & Mijaíl Sazhín
 * @date 03-2025
*/
#include "socket.h"

/* ---------------------- Public Functions ---------------------- */
void free_socket(S_socket *s_socket) {
    if (s_socket != NULL) {
        close(s_socket->socket);
        free(s_socket);
    }
}

S_socket* init_socket(int port) {
    struct sockaddr_in address;
    int sock;
    int status;
    int opt = 1;
    S_socket *s_socket;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("socket");
        return NULL;
    }

    status = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    status = bind(sock, (struct sockaddr *) &address, sizeof(address));
    if (status != 0) {
        perror("bind");
        return NULL;
    }

    status = listen(sock, 1);
    if (status < 0) {
        perror("listen");
        return NULL;
    }

    s_socket = (S_socket*)malloc(sizeof(S_socket));
    if (s_socket == NULL) {
        return NULL;
    }

    s_socket->address = address;
    s_socket->socket = sock;

    return s_socket;
}
