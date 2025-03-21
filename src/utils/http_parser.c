/**
 * @file http_parser.c
 * @brief HTTP request parser implementation.
 *
 * This file contains the implementation of an HTTP request parser, which
 * processes HTTP requests and extracts relevant information such as the
 * method, filename, arguments, and HTTP version.
 * 
 * @author Miguel Paterson & Mijaíl Sazhín
 * @date 03-2025
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "http_parser.h"

/* ---------------------- Private Functions ---------------------- */
/**
 * @brief Initializes a new Parser structure.
 *
 * Allocates memory for a new Parser structure and its fields, and initializes
 * its members to default values.
 *
 * @return Pointer to the newly allocated Parser structure, or NULL if memory
 *         allocation fails.
 */
Parser *_init_parser() {
    Parser *parser;
    parser = (Parser*) malloc(sizeof(Parser));
    if (parser == NULL) {
        perror("malloc");
        return NULL;
    }

    parser->filename = (char*)malloc(MAX_PATH);
    if (parser->filename == NULL) {
        perror("malloc");
        free(parser);
        return NULL;
    }
    parser->args = (char*)malloc(MAX_ARGS);
    if (parser->args == NULL) {
        perror("malloc");
        free(parser);
        return NULL;
    }

    parser->method = UNKNOWN;
    parser->status = HTTP_OK;

    return parser;
}


/* ---------------------- Public Functions ---------------------- */
void free_parser(Parser *parser) {
    if (parser != NULL) {
        if (parser->filename != NULL) {
            free(parser->filename);
            parser->filename = NULL;
        }
        if (parser->args != NULL) {
            if (parser->method == POST) {
                parser->args -= 4;
            }
            free(parser->args);
            parser->args = NULL;
        }
        free(parser);
    }
    parser = NULL;
    return;
}

Parser *pars_http(char *petition, Dict *conf) {
    Parser *parser;
    FILE *file = NULL;
    char method[MAX_METHOD], *temp, *query_string = NULL, *args, *temp2, version[MAX_VERSION];

    parser = _init_parser();
    if (parser == NULL) {
        return NULL;
    }

    printf("PETITION: %s\n", petition);

    if (sscanf(petition, "%7s %256s %8s", method, parser->filename, version) < 0) {
        free_parser(parser);
        return NULL;
    }

    query_string = strstr(parser->filename, "?");

    if (query_string != NULL) {
        args = query_string + 1;
        sprintf(parser->args, "%s", args);

        size_t length = query_string - parser->filename;
        printf("query_string: %p\n", query_string);
        printf("parser->filename: %p\n", parser->filename);
        temp2 = (char*)malloc(length + 1);
        if (temp2 == NULL) {
            perror("Malloc");
        }
        strncpy(temp2, parser->filename, length);
        temp2[length] = '\0';
        strcpy(parser->filename, temp2);
        free(temp2);
    }

    if (strlen(parser->filename) == 0 || strlen(version) == 0) {
        parser->status = HTTP_BAD_REQUEST;
        return parser;
    }

    if (strcmp(parser->filename, "/") == 0) {
        strcpy(parser->filename, get_value(conf, "INDEX_FILE"));
    } else {
        char *base_dir = get_value(conf, "BASE_DIR");
        if (base_dir == NULL) {
            free_parser(parser);
            return NULL;
        }
        temp = (char*)calloc(sizeof(char), strlen(parser->filename) + 1);
        if (temp == NULL) {
            free_parser(parser);
            return NULL;
        }

        strcpy(temp, parser->filename);
        strcpy(parser->filename, base_dir);
        strcat(parser->filename, temp);
        printf("FILENAME: %s\n", parser->filename);
        free(temp);
    }

    printf("METHOD: %s\n", method);
    if (strcmp(method, "GET") == 0) {
        parser->method = GET;
        file = fopen(parser->filename, "rb");
    } else if (strcmp(method, "POST") == 0) {
        parser->method = POST;
        file = fopen(parser->filename, "rb");
    } else if (strcmp(method, "OPTIONS") == 0) {
        parser->method = OPTIONS;
        file = fopen(parser->filename, "rb");
    } else {
        parser->method = UNKNOWN;
    }

    if (file == NULL) {
        parser->status = HTTP_NOT_FOUND;
        parser->type = UNKNOWN;
    } else {
        parser->status = HTTP_OK;
        parser->type = get_file_type(parser->filename);
        fclose(file);
    }

    temp = strrchr(version, '.');
    if (strcmp(temp, ".1") == 0) {
        parser->version = HTTP1_1;
    } else {
        parser->version = HTTP1_0;
    }

    printf("METHOD: %d\n", parser->method);
    printf("FILENAME: %s\n", parser->filename);
    printf("TYPE: %d\n", parser->type);
    printf("STATUS: %d\n", parser->status);
    return parser;
}
