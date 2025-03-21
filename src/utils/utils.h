/**
 * @file utils.h
 * @brief Utility functions and definitions for file handling and HTTP methods.
 *
 * This header file provides utility functions for working with files, determining
 * file types, and handling HTTP methods. It also defines enums for file types
 * and HTTP methods.
 * 
 * @author Miguel Paterson & Mijaíl Sazhín
 * @date 03-2025
 */

#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#define BUFFER_SIZE 4096

/**
 * @enum File_type
 * @brief Enumeration of supported file types.
 *
 * This enum defines various file types that can be used to identify
 * the type of a file based on its extension or content.
 */
typedef enum {
    JPG,        /**< JPEG image file */
    HTML,       /**< HTML file */
    TEXT,       /**< Plain text file */
    BINARY,     /**< Binary file */
    GIF,        /**< GIF image file */
    MPEG,       /**< MPEG video file */
    PHP,        /**< PHP script file */
    PYTHON,     /**< Python script file */
    UNKNOWN,    /**< Unknown file type */
    MP4         /**< MP4 video file */
} File_type;

/**
 * @enum Method
 * @brief Enumeration of supported HTTP methods.
 *
 * This enum defines the HTTP methods that can be handled by the utility functions.
 */
typedef enum {
    GET,            /**< HTTP GET method */
    POST,           /**< HTTP POST method */
    OPTIONS,        /**< HTTP OPTIONS method */
    UNKNOWN_METHOD  /**< Unknown HTTP method */
} Method;

/**
 * @brief Opens a file and returns its content.
 *
 * @param filename The name of the file to open.
 * @param type The type of the file (from the File_type enum).
 * @return A pointer to the file content, or NULL on failure.
 */
void *open_file(char *filename, File_type type);

/**
 * @brief Determines the type of a file based on its name or content.
 *
 * @param filename The name of the file to analyze.
 * @return The file type (from the File_type enum).
 */
File_type get_file_type(char *filename);

/**
 * @brief Gets the size of a file in bytes.
 *
 * @param filename The name of the file.
 * @return The size of the file in bytes, or 0 on failure.
 */
size_t get_file_size(const char *filename);

/**
 * @brief Opens a script file and processes it based on the HTTP method and input.
 *
 * @param filename The name of the script file to open.
 * @param type The type of the file (from the File_type enum).
 * @param method The HTTP method to use (from the Method enum).
 * @param input The input data to pass to the script.
 * @return A pointer to the processed script output, or NULL on failure.
 */
char *open_script(char *filename, File_type type, Method method, char *input);

#endif