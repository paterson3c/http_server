/**
 * @file socket.h
 * @brief Header file for socket utility functions and structures.
 *
 * This file contains the definitions and declarations for managing sockets,
 * including initialization and cleanup, as well as server type enumeration.
 * 
 * @author Miguel Paterson & Mijaíl Sazhín
 * @date 03-2025
 */

#ifndef SOCKET_H
#define SOCKET_H

#include <stdio.h>
#include <omp.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#define SERV_PORT 8080

/**
 * @struct S_socket
 * @brief Represents a server socket.
 *
 * This structure stores information about a server socket, including its file descriptor
 * and associated network address.
 */
typedef struct {
    int socket;                /**< File descriptor for the socket. */
    struct sockaddr_in address; /**< Socket address structure containing IP and port information. */
} S_socket;

/**
 * @brief Initializes a socket and binds it to the specified port.
 *
 * This function creates a socket, binds it to the given port, and prepares it
 * for use in communication.
 *
 * @param port The port number to bind the socket to.
 * @return A pointer to an S_socket structure representing the initialized socket.
 */
S_socket* init_socket(int port);

/**
 * @brief Frees the resources associated with a socket.
 *
 * This function closes the socket and releases any memory allocated for the
 * S_socket structure.
 *
 * @param s_socket A pointer to the S_socket structure to be freed.
 */
void free_socket(S_socket *s_socket);

#endif