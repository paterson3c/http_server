/**
 * @file conf_parser.h
 * @brief Header file for configuration file parsing utilities.
 *
 * This file provides the definitions and function declarations for parsing
 * configuration files into a dictionary-like structure. It includes functions
 * for retrieving values by keys, freeing allocated memory, and parsing
 * configuration files into a structured format.
 *
 * The configuration file is expected to contain key-value pairs separated by
 * an '=' character. Lines starting with '#' or empty lines are ignored during
 * parsing.
 *
 * @author Miguel Paterson & Mijaíl Sazhín
 * @date 03-2025
 */
#ifndef CONF_PARSER_H
#define CONF_PARSER_H
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_LINE 2048
#define MAX_ELEMS 20

/**
 * @brief Represents a key-value pair element.
 * 
 * This structure is used to store a single key-value pair,
 * where both the key and value are strings.
 */
typedef struct{
    char *key;   /**< Pointer to the key string. */
    char *value; /**< Pointer to the value string. */
} Elem;

/**
 * @brief Represents a dictionary of key-value pairs.
 * 
 * This structure is used to store a collection of key-value pairs
 * as an array of pointers to `Elem` structures.
 */
typedef struct{
    Elem **dict; /**< Array of pointers to `Elem` structures. */
    int size;    /**< Number of elements in the dictionary. */
} Dict;

/**
 * @brief Frees the memory allocated for a Dict structure and its elements.
 *
 * @param dict Pointer to the Dict structure to be freed.
 */
void free_dict(Dict *dict);

/**
 * @brief Retrieves the value associated with a key in a Dict structure.
 *
 * Searches the dictionary for the specified key and returns the corresponding value.
 *
 * @param dict Pointer to the Dict structure.
 * @param key Pointer to the key string.
 * @return Pointer to the value string, or NULL if the key is not found.
 */
char *get_value(Dict *dict, char *key);

/**
 * @brief Parses a configuration file into a Dict structure.
 *
 * Reads a configuration file line by line, extracting key-value pairs and storing
 * them in a dictionary. Lines starting with '#' or empty lines are ignored.
 *
 * @param filename Pointer to the name of the configuration file.
 * @return Pointer to the populated Dict structure, or NULL on failure.
 */
Dict *conf_parse(char *filename);

#endif