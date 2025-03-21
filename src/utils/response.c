/**
 * @file response.c
 * @brief Implementation of utility functions for handling responses in the application.
 *
 * This file contains the implementation of functions related to processing
 * and managing responses. It includes necessary headers for response handling
 * and socket operations.
 *
 * @author Miguel Paterson & Mijaíl Sazhín
 * @date 03-2025
 */
 
#include "response.h"
#include "socket.h"

/* ---------------------- Private Functions ---------------------- */
/**
 * @brief Initializes a new Response object.
 * 
 * This function allocates and initializes a new Response structure.
 * It sets up the necessary fields and prepares the object for use.
 * 
 * @return A pointer to the newly created Response object, or NULL if
 *         memory allocation fails.
 */
Response *_init_response() {
    Response *response;
    response = (Response*)malloc(sizeof(Response));
    if (response == NULL) {
        return NULL;
    }
    response->content = NULL;
    response->header = NULL;
    response->content_length = 0;
    return response;
}


/**
 * @brief Creates an OPTIONS HTTP header based on the specified file type.
 *
 * This function generates an OPTIONS header string that specifies the allowed
 * HTTP methods for a given file type. The generated header can be used in
 * HTTP responses to indicate the supported operations for the resource.
 *
 * @param type The file type for which the OPTIONS header is being created.
 *             This parameter is expected to be of type `File_type`.
 *
 * @return A dynamically allocated string containing the OPTIONS header.
 *         The caller is responsible for freeing the returned string.
 */
char *_create_OPTIONS_header(File_type type) {
    char *header = NULL;
    header = (char*)calloc(sizeof(char), 100);
    if (header == NULL) {
        return NULL;
    }
    if (type == HTML) {
        sprintf(header, "HTTP/1.1 200 OK\r\n"
            "Content-Type: html; charset=UTF-8\r\n"
            "Allow: GET, OPTIONS\r\n"
            "\r\n");
    } else if (type == JPG) {
        sprintf(header, "HTTP/1.1 200 OK\r\n"
            "Content-Type: image/jpeg\r\n"
            "Allow: GET, OPTIONS\r\n"
            "\r\n");
    } else if (type == PYTHON || type == PHP) {
        sprintf(header, "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/plain; charset=UTF-8\r\n"
            "Allow: GET, POST, OPTIONS\r\n"
            "\r\n");
    } else if (type == GIF) {
        sprintf(header, "HTTP/1.1 200 OK\r\n"
            "Content-Type: image/gif\r\n"
            "Allow: GET, OPTIONS\r\n"
            "\r\n");
    } else if (type == MPEG) {
        sprintf(header, "HTTP/1.1 200 OK\r\n"
            "Content-Type: video/mpeg\r\n"
            "Allow: GET, OPTIONS\r\n"
            "\r\n");
    } else if (type == TEXT) {
        sprintf(header, "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/plain; charset=UTF-8\r\n"
            "Allow: GET, OPTIONS\r\n"
            "\r\n");
    } else if (type == MP4) {
        sprintf(header, "HTTP/1.1 200 OK\r\n"
            "Content-Type: video/mp4\r\n"
            "Allow: GET, OPTIONS\r\n"
            "\r\n");
    }
    return header;
}


/**
 * @brief Creates an HTTP GET response header.
 *
 * This function generates a properly formatted HTTP GET response header
 * based on the provided file information, including the filename, file size,
 * and file type.
 *
 * @param filename The name of the file being requested.
 * @param file_size The size of the file in bytes.
 * @param type The type of the file (e.g., text, image, etc.).
 * @return A dynamically allocated string containing the HTTP GET response header.
 *         The caller is responsible for freeing the allocated memory.
 */
char *_create_GET_header(char *filename, long file_size, File_type type) {
    char *header = NULL;
    header = (char*)calloc(sizeof(char), 200);
    if (header == NULL) {
        return NULL;
    }
    if (type == HTML) {
        sprintf(header, "HTTP/1.1 200 OK\r\n"
            "Content-Type: html; charset=UTF-8\r\n"
            "Content-Length: %ld\r\n"
            "\r\n", file_size);
    } else if (type == JPG) {
        sprintf(header, "HTTP/1.1 200 OK\r\n"
            "Content-Type: image/jpeg\r\n"
            "Content-Length: %ld\r\n"
            "\r\n", file_size);
    } else if (type == TEXT || type == PYTHON || type == PHP) {
        sprintf(header, "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/plain; charset=UTF-8\r\n"
            "Content-Length: %ld\r\n"
            "\r\n", file_size);
    } else if (type == GIF) {
        sprintf(header, "HTTP/1.1 200 OK\r\n"
            "Content-Type: image/gif\r\n"
            "Content-Length: %ld\r\n"
            "\r\n", file_size);
    } else if (type == MPEG) {
        sprintf(header, "HTTP/1.1 200 OK\r\n"
            "Content-Type: video/mpeg\r\n"
            "Content-Length: %ld\r\n"
            "\r\n", file_size);
    } else if (type == MP4) {
        sprintf(header, "HTTP/1.1 200 OK\r\n"
            "Content-Type: video/mp4\r\n"
            "Content-Length: %ld\r\n"
            "Content-Disposition: attachment; filename=\"video.mp4\"\r\n"
            "\r\n", file_size);
    }
    return header;
}

/**
 * @brief Creates a POST request header for a given file.
 *
 * This function generates a POST request header string based on the provided
 * filename, file size, and file type. The generated header can be used in
 * HTTP requests to indicate the file being sent and its associated metadata.
 *
 * @param filename The name of the file to be included in the POST request.
 *                 This should be a null-terminated string.
 * @param file_size The size of the file in bytes.
 * @param type The type of the file, represented as a `File_type` enum or similar.
 * 
 * @return A dynamically allocated string containing the POST request header.
 *         The caller is responsible for freeing the memory allocated for the
 *         returned string.
 */

char *_create_POST_header(char *filename, long file_size, File_type type) {
    char *header = NULL;
    header = (char*)calloc(sizeof(char), 100);
    if (header == NULL) {
        return NULL;
    }
    sprintf(header, "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain; charset=UTF-8\r\n"
        "Content-Length: %ld\r\n"
        "\r\n", file_size);
    return header;
}


/**
 * @brief Creates a 404 HTTP response header.
 *
 * This function generates a 404 Not Found response header based on the provided
 * parser object. The generated header can be used to inform the client that the
 * requested resource could not be found on the server.
 *
 * @param parser A pointer to a Parser object containing relevant information
 *               for constructing the response header.
 * @return A dynamically allocated string containing the 404 response header.
 *         The caller is responsible for freeing the allocated memory.
 */
char *_create_404_header(Parser *parser) {
    char *header = NULL;
    char *body = "<!DOCTYPE html>\n"
        "<html>\n"
        "<head>\n"
        "<title>404 Not Found</title>\n"
        "</head>\n"
        "<body>\n"
        "<h1>Error 404 Not Found</h1>\n"
        "</body>\n"
        "</html>\n";
    int body_length = strlen(body);
    header = (char*)calloc(sizeof(char), 300 + body_length);
    if (header == NULL) {
        return NULL;
    }
    sprintf(header, "HTTP/1.1 404 Not Found\r\n"
        "Content-Type: html; charset=UTF-8\r\n"
        "Content-Length: %d\r\n"
        "\r\n", body_length);
    strcat(header, body);
    return header;
}


/**
 * @brief Creates a "Bad Request" HTTP response based on the provided parser.
 *
 * This function generates a response string indicating a "400 Bad Request" error.
 * It uses the information from the given parser to construct the response.
 *
 * @param parser A pointer to a Parser structure containing the necessary data
 *               to build the "Bad Request" response.
 * @return A dynamically allocated string containing the "Bad Request" response.
 *         The caller is responsible for freeing the allocated memory.
 */
char *_create_BadRequest_response(Parser *parser) {
    char *header = NULL;
    char *body = "<!DOCTYPE html>\n"
        "<html>\n"
        "<head>\n"
        "<title>400 Bad Request</title>\n"
        "</head>\n"
        "<body>\n"
        "<h1>Error 400 Bad Request</h1>\n"
        "</body>\n"
        "</html>\n";
    int body_length = strlen(body);
    header = (char*)calloc(sizeof(char), body_length + 300);
    if (header == NULL) {
        return NULL;
    }
    sprintf(header, "HTTP/1.1 400 Bad Request\r\n"
        "Content-Type: html; charset=UTF-8\r\n"
        "Content-Length: %d\r\n"
        "\r\n", body_length);
    strcat(header, body);
    return header;
}


/* ---------------------- Public Functions ---------------------- */
void send_file(int socket_fd, Response *response) {
    size_t bytes_sent = 0;
    size_t total_size = response->content_length;
    char *content_ptr = (char *)response->content;
    size_t bytes_to_send = 0;
    ssize_t status;

    while (bytes_sent < total_size) {
        if ((total_size - bytes_sent) > BUFFER_SIZE)
            bytes_to_send = BUFFER_SIZE;
        else
            bytes_to_send = total_size - bytes_sent;

        status = send(socket_fd, content_ptr + bytes_sent, bytes_to_send, 0);

        if (status == -1) {
            perror("send");
            return;
        }
        bytes_sent += status;
    }
}


void free_response(Response *response) {
    if (response != NULL) {
        if (response->content != NULL) {
            free(response->content);
            response->content = NULL;
        }
        if (response->header != NULL) {
            free(response->header);
            response->header = NULL;
        }
        free(response);
    }
    response = NULL;
    return;
}


Response *create_response(Parser *parser) {
    Response *response;
    char *header;
    void *file;
    response = _init_response();
    if (response == NULL) {
        return NULL;
    }
    if (parser->status == HTTP_NOT_FOUND) {
        header = _create_404_header(parser);
        if (header == NULL) {
            free_response(response);
            return NULL;
        }
        response->header = header;
        return response;
    }
    if (parser->status == HTTP_BAD_REQUEST) {
        header = _create_BadRequest_response(parser);
        if (header == NULL) {
            free_response(response);
            return NULL;
        }
        response->header = header;
        return response;
    }
    if (parser->method == OPTIONS) {
        header = _create_OPTIONS_header(parser->type);
        response->content = NULL;
        response->header = header;
        return response;
    }
    file = NULL;
    if (parser->type != PYTHON && parser->type != PHP) {
        file = open_file(parser->filename, parser->type);
    } else if (parser->method != OPTIONS) {
        printf("Opening script with %s\n", parser->args);
        file = open_script(parser->filename, parser->type, parser->method, parser->args);
    }
    if (file == NULL) {
        printf("Error al abrir el archivo\n");
        free_response(response);
        return NULL;
    }
    printf("Archivo abierto\n");
    if (parser->type == TEXT || parser->type == HTML || parser->type == PYTHON || parser->type == PHP) {
        response->content_length = strlen((char*)file);
    } else if (parser->type == BINARY || parser->type == JPG || parser->type == GIF || parser->type == MPEG || parser->type == MP4) {
        response->content_length = get_file_size(parser->filename);
    } else {
        free(file);
        free_response(response);
        return NULL;
    }
    if (parser->method == GET) {
        header = _create_GET_header(parser->filename, response->content_length, parser->type);
        response->content = file;
    } else if (parser->method == POST) {
        header = _create_POST_header(parser->filename, response->content_length, parser->type);
        response->content = file;
    } else {
        free(file);
        free_response(response);
        return NULL;
    }
    if (header == NULL) {
        free(file);
        free_response(response);
        return NULL;
    }
    printf("Header creado\n");
    response->header = header;
    return response;
}
