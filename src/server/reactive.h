/**
 * @file reactive.h
 * @brief Header file for the reactive server implementation.
 *
 * This file contains the declarations of functions and constants used for implementing
 * a reactive server. The server handles incoming client connections by creating a new
 * thread for each client, enforcing a maximum number of simultaneous clients, and
 * ensuring graceful shutdown.
 *
 * The reactive server relies on utility modules for socket handling, configuration
 * parsing, HTTP parsing, and response generation.
 *
 * @author Miguel Paterson & Mijaíl Sazhín
 * @date 03-2025
 */

#ifndef REACTIVE_H
#define REACTIVE_H

#define MAX_THREADS 1024

#include "../utils/socket.h"
#include "../utils/conf_parser.h"
#include "../utils/http_parser.h"
#include "../utils/utils.h"
#include "../utils/response.h"

/**
 * @brief Initializes signal handlers for the server.
 *
 * This function sets up signal handlers for SIGINT (graceful shutdown) and SIGPIPE (ignored).
 * It also blocks the SIGALRM signal in the main thread to ensure proper handling in client threads.
 *
 * @return 0 on success, -1 on failure.
 */
int init_handler();

/**
 * @brief Waits for all client threads to finish and cleans up resources.
 *
 * This function waits for all active client threads to terminate and releases global resources
 * such as the server socket and configuration dictionary.
 */
void cleanup_threads();

/**
 * @brief Starts the server and listens for incoming client connections.
 *
 * This function initializes the server, listens for incoming client connections, and creates
 * a new thread for each client. It enforces a maximum number of simultaneous clients and handles
 * graceful shutdown when the shutdown flag is set.
 *
 * @param e_s_socket Pointer to the server socket structure.
 * @param e_conf Pointer to the configuration dictionary.
 * @return 0 on success, -1 on failure.
 */
int server_listen(S_socket *e_s_socket, Dict *e_conf);

#endif