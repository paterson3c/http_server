/**
 * @file response.h
 * @brief Header file for handling HTTP responses.
 *
 * This file contains the definition of the Response structure and the
 * declarations of functions used to create, manage, and send HTTP responses.
 * 
 * @author Miguel Paterson & Mijaíl Sazhín
 * @date 03-2025
 */

#ifndef RESPONSE_H
#define RESPONSE_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "http_parser.h"
#include "utils.h"

/**
 * @struct Response
 * @brief Represents an HTTP response.
 *
 * This structure stores the data of an HTTP response, including its content, 
 * associated HTTP headers, and the length of the content.
 */
typedef struct {
    void *content;         /**< Pointer to the response content (can be text or binary data). */
    char *header;          /**< HTTP headers of the response (includes metadata such as Content-Type). */
    size_t content_length; /**< Size of the content in bytes. */
} Response;


/**
 * @brief Creates a Response object based on the parsed HTTP request.
 *
 * @param parser Pointer to the Parser object containing the parsed HTTP request.
 * @return Pointer to the newly created Response object.
 */
Response *create_response(Parser *parser);

/**
 * @brief Frees the memory allocated for a Response object.
 *
 * @param response Pointer to the Response object to be freed.
 */
void free_response(Response *response);

/**
 * @brief Sends a file as part of the HTTP response.
 *
 * @param socket_fd File descriptor of the socket to send the response through.
 * @param response Pointer to the Response object containing the file to be sent.
 */
void send_file(int socket_fd, Response *response);

#endif