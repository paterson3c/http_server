/**
 * @file conf_parser.c
 * @brief Configuration file parser utility functions.
 *
 * This file contains functions to parse configuration files into a dictionary-like
 * structure, as well as utility functions for memory management and string manipulation.
 * 
 * @author Miguel Paterson & Mijaíl Sazhín
 * @date 03-2025
 */

#include "conf_parser.h"
#include <ctype.h>

/* ---------------------- Private Functions ---------------------- */
/**
 * @brief Frees the memory allocated for an Elem structure.
 *
 * @param elem Pointer to the Elem structure to be freed.
 */
void _free_elem(Elem *elem) {
    if (elem != NULL) {
        if (elem->key != NULL) {
            free(elem->key);
        }
        if (elem->value != NULL) {
            free(elem->value);
        }
        free(elem);
    }
    return;
}

/**
 * @brief Initializes a new Dict structure.
 *
 * Allocates memory for a Dict structure and initializes its fields.
 *
 * @return Pointer to the initialized Dict structure, or NULL on failure.
 */
Dict *_init_dict() {
    Dict *dict = NULL;

    dict = (Dict*)malloc(sizeof(Dict));
    if (dict == NULL) {
        return NULL;
    }

    dict->dict = NULL;
    dict->size = 0;

    return dict;
}

/**
 * @brief Adds a key-value pair to a Dict structure.
 *
 * Allocates memory for a new entry and appends it to the dictionary.
 *
 * @param dict Pointer to the Dict structure.
 * @param key Pointer to the key string.
 * @param value Pointer to the value string.
 * @return Pointer to the updated Dict structure, or NULL on failure.
 */
Dict *_add_entry(Dict *dict, char *key, char *value) {
    if (dict == NULL) {
        return NULL;
    }

    dict->dict = (Elem**)realloc(dict->dict, (dict->size + 1) * sizeof(Elem*));
    if (dict->dict == NULL) {
        free_dict(dict);
        return NULL;
    }

    dict->dict[dict->size] = (Elem*)malloc(sizeof(Elem));
    if (dict->dict[dict->size] == NULL) {
        free_dict(dict);
        return NULL;
    }

    dict->dict[dict->size]->key = strdup(key);
    dict->dict[dict->size]->value = strdup(value);

    dict->size++;

    return dict;
}

/**
 * @brief Trims leading and trailing whitespace from a string.
 *
 * Removes all leading and trailing whitespace characters from the input string
 * and compacts the string in place.
 *
 * @param str Pointer to the string to be trimmed.
 */
void _trim(char *str) {
    char *start = str, *end, *dst = str;

    while (*start && isspace((unsigned char)*start)) {
        start++;
    }

    if (*start == '\0') {
        *str = '\0';
        return;
    }

    end = start + strlen(start) - 1;
    while (end > start && isspace((unsigned char)*end)) {
        end--;
    }

    while (start <= end) {
        if (!isspace((unsigned char)*start)) {
            *dst++ = *start;
        }
        start++;
    }

    *dst = '\0';
}


/* ---------------------- Public Functions ---------------------- */
void free_dict(Dict *dict) {
    int i = 0;

    if (dict != NULL) {
        if (dict->dict != NULL) { 
            for (i = 0; i < dict->size; i++)
                _free_elem(dict->dict[i]);
            free(dict->dict);
        }
        free(dict);
    }

    dict = NULL;
    return;
}

char *get_value(Dict *dict, char *key) {
    int i;
    for (i = 0; i < dict->size; i++) {
        if (strcmp(dict->dict[i]->key, key) == 0)
            return dict->dict[i]->value;
    }
    return NULL;
}

Dict *conf_parse(char *filename) {
    FILE *file = NULL;
    Dict *dict = NULL; 
    char line[MAX_LINE];
    char key[MAX_LINE], value[MAX_LINE];

    file = fopen(filename, "r");
    if (file == NULL) {
        return NULL;
    }

    dict = _init_dict();
    if (dict == NULL) {
        fclose(file);
        return NULL;
    }

    while (1) {
        if (fgets(line, sizeof(line), file) == NULL) {
            if (ftell(file) == 0)
                printf("archivo vacio");
            break;
        }

        _trim(line); 
        if (line[0] == '#' || strlen(line) == 0) continue;  

        if (sscanf(line, "%[^=]=%s", key, value) == 2) { 
            _trim(key);
            _trim(value);
            dict = _add_entry(dict, key, value);
            if (dict == NULL) {
                fclose(file);
                return NULL;
            }
        }
    }

    fclose(file);
    return dict;
}

