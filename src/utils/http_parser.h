/**
 * @file http_parser.h
 * @brief Header file for HTTP request parsing utilities.
 *
 * This file contains the definitions and function declarations for parsing
 * HTTP requests and managing the parsed data.
 * 
 * @author Miguel Paterson & Mijaíl Sazhín
 * @date 03-2025
 */

#ifndef HTTP_PARSER_H
#define HTTP_PARSER_H

#define MAX_METHOD 8
#define MAX_VERSION 9
#define MAX_PATH 256
#define MAX_ARGS 1024

#include "conf_parser.h"
#include "utils.h"

/**
 * @enum HttpStatusCode
 * @brief Enumeration of HTTP status codes.
 *
 * This enum defines common HTTP status codes used in the parser.
 */
typedef enum{
    HTTP_OK = 200,          /**< HTTP 200 OK */
    HTTP_BAD_REQUEST = 400, /**< HTTP 400 Bad Request */
    HTTP_NOT_FOUND = 404    /**< HTTP 404 Not Found */
} HttpStatusCode;

/**
 * @enum Version
 * @brief Enumeration of HTTP protocol versions.
 *
 * This enum defines supported HTTP protocol versions.
 */
typedef enum{
    HTTP1_0, /**< HTTP/1.0 */
    HTTP1_1  /**< HTTP/1.1 */
} Version;

/**
 * @struct Parser
 * @brief Structure to hold parsed HTTP request data.
 *
 * This structure contains the parsed components of an HTTP request,
 * including the method, filename, arguments, file type, status, and version.
 */
typedef struct{
    Method method;          /**< HTTP method (e.g., GET, POST) */
    char *filename;         /**< Requested file name */
    char *args;             /**< Query string arguments */
    File_type type;         /**< File type of the requested resource */
    HttpStatusCode status;  /**< HTTP status code */
    Version version;        /**< HTTP protocol version */
} Parser;

/**
 * @brief Parses an HTTP request.
 *
 * This function parses an HTTP request string and populates a Parser structure
 * with the extracted data.
 *
 * @param petition The HTTP request string to parse.
 * @param conf A dictionary containing configuration data.
 * @return A pointer to a dynamically allocated Parser structure containing
 *         the parsed data. The caller is responsible for freeing this memory
 *         using free_parser().
 */
Parser *pars_http(char *petition, Dict *conf);

/**
 * @brief Frees memory allocated for a Parser structure.
 *
 * This function releases the memory allocated for a Parser structure and its
 * associated fields.
 *
 * @param parser A pointer to the Parser structure to free.
 */
void free_parser(Parser *parser);

#endif