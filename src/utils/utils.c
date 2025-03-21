/**
 * @file utils.c
 * @brief Utility functions for network-related operations.
 *
 * This file contains the implementation of utility functions that assist
 * in socket programming, process management, and signal handling.
 *
 * @author Miguel Paterson & Mijaíl Sazhín
 * @date 03-2025
 */

#include "utils.h"
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

/* ---------------------- Private Functions ---------------------- */
/**
 * @brief Replaces all occurrences of the '&' character in a string with a space (' ').
 *
 * This function iterates through the given string and replaces every
 * ampersand ('&') character with a space (' '). The modification is done
 * in place, directly altering the input string.
 *
 * @param str Pointer to the null-terminated string to be modified.
 *            The string must be writable.
 */
void _replace_ampersand(char *str) {
    while (*str) {
        if (*str == '&') {
            *str = ' ';  
        }
        str++;
    }
}


/* ---------------------- Public Functions ---------------------- */
void *open_file(char *filename, File_type type){
    FILE *file;
    void *content;
    int tam;
    size_t file_size;

    // Checks the type of the file to open
    if (type == BINARY || type == JPG || type == GIF || type == MPEG || type == MP4){
        file = fopen(filename, "rb");
    }
    else if (type == TEXT || type == HTML){
        file = fopen(filename, "r");
    }
    else{
        return NULL;
    }
    if (file == NULL){
        perror("fopen");
        return NULL;
    }


    // Gets the size of the file
    file_size = get_file_size(filename);
    if (file_size == 0){
        fclose(file);
        return NULL;
    }

    // Reads the file and stores it on a string or binary
    if (type == TEXT || type == HTML){
        content = (char*)calloc(sizeof(char), file_size);
        if (content == NULL){
            fclose(file);
            return NULL;
        }

        tam = fread(content, sizeof(char), file_size - 1, file);
        if (tam < 0){
            fclose(file);
            free(content);
            perror("fread");
            return NULL;
        }
    }
    else if (type == BINARY || type == JPG || type == GIF || type == MPEG || type == MP4){
        content = malloc(file_size);
        if (content == NULL){
            fclose(file);
            return NULL;
        }

        tam = fread(content, 1, file_size, file);
        if (tam < 0){
            fclose(file);
            free(content);
            perror("fread");
            return NULL;
        }
    }

    fclose(file);

    return content;
}


char *open_script(char *filename, File_type type, Method method, char *input) {
    FILE *file_out;
    char command[256];
    char buffer[1024];
    char *output = NULL;
    size_t output_size = 0;
    int fdfather[2], fdson[2];
    pid_t pid;

    _replace_ampersand(input);
    printf("ARGS: %s\n", input);

    if (input == NULL) {
        return NULL;
    }

    if (method == GET) {
        // Construir el comando para GET
        if (type == PYTHON) {
            snprintf(command, sizeof(command), "python3 %s %s", filename, input);
        } else if (type == PHP) {
            snprintf(command, sizeof(command), "php %s %s", filename, input);
        } else {
            return NULL;
        }

        file_out = popen(command, "r");  // Ejecutar el script y leer su salida
        if (!file_out) {
            perror("popen");
            return NULL;
        }

        while (fgets(buffer, sizeof(buffer), file_out)) {
            size_t len = strlen(buffer);
            char *new_output = realloc(output, output_size + len + 1);
            if (!new_output) {
                free(output);
                pclose(file_out);
                return NULL;
            }
            output = new_output;
            memcpy(output + output_size, buffer, len);
            output_size += len;
        }
        
        if (output) {
            output[output_size] = '\0'; // Asegurar terminación en null
        }

        pclose(file_out);
        return output;
    } 
    else if (method == POST) {
        if (pipe(fdfather) == -1 || pipe(fdson) == -1) {
            perror("pipe");
            return NULL;
        }

        pid = fork();
        if (pid == -1) {
            perror("fork");
            return NULL;
        }

        if (pid == 0) { 
            close(fdson[1]);  
            close(fdfather[0]); 

            dup2(fdson[0], STDIN_FILENO); 
            close(fdson[0]);

            dup2(fdfather[1], STDOUT_FILENO); 
            close(fdfather[1]);

            // Ejecutar el script
            if (type == PYTHON) {
                execlp("python3", "python3", filename, NULL);
            } else if (type == PHP) {
                execlp("php", "php", filename, NULL);
            }
            
            perror("execlp");
            exit(EXIT_FAILURE);
        } 
        else { 
            close(fdson[0]);
            close(fdfather[1]);

            // Enviar input al hijo
            write(fdson[1], input, strlen(input));
            close(fdson[1]); // Indicar fin de datos

            // Leer la salida del hijo
            ssize_t n;
            while ((n = read(fdfather[0], buffer, sizeof(buffer) - 1)) > 0) {
                buffer[n] = '\0'; 
                size_t len = n;
                char *new_output = realloc(output, output_size + len + 1);
                if (!new_output) {
                    free(output);
                    close(fdfather[0]);
                    return NULL;
                }
                output = new_output;
                memcpy(output + output_size, buffer, len);
                output_size += len;
            }

            if (output) {
                output[output_size] = '\0'; 
            }

            close(fdfather[0]);
            wait(NULL); // Esperar a que el hijo termine
            return output;
        }
    }
    
    return NULL;
}


File_type get_file_type(char *filename){
    char *ext;

    ext = strrchr(filename, '.');
    if (strcmp(ext, ".html") == 0){
        return HTML;
    }
    else if (strcmp(ext, ".txt") == 0){
        return TEXT;
    }
    else if (strcmp(ext, ".gif") == 0){
        return GIF;
    }
    else if (strcmp(ext, ".jpg") == 0 || strcmp(ext, ".jpeg") == 0 || strcmp(ext, ".ico") == 0){
        return JPG;
    }
    else if (strcmp(ext, ".mpeg") == 0){
        return MPEG;
    }
    else if (strcmp(ext, ".py") == 0){
        return PYTHON;
    }
    else if (strcmp(ext, ".php") == 0){
        return PHP;
    }
    else if (strcmp(ext, ".mp4") == 0){
        return MP4;
    }
    return UNKNOWN;
}


size_t get_file_size(const char *filename) {
    struct stat st;
    if (stat(filename, &st) == 0)
        return st.st_size;
    return 0; 
}
