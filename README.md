# Práctica 1 Redes de Comunicaciones 2

## Ejecución
**IMPORTANTE**: antes de empezar con la ejecución comprobar que existan o crear los directorios bin y obj en la raíz del proyecto. Además se deberán añadir los archivos de media necesarios, lea el archivo index.html para poder escoger los elementos necesarios.
Mediante el archivo `Makefile`, se pueden ejecutar las siguientes opciones:

- `make`: Borra archivos `.o` y genera nuevos ejecutables.
- `make run_server`: Ejecuta el servidor con la configuración por defecto.
- `make run_client`: Ejecuta el cliente con la IP por defecto.
- `make valgrind_server`: Ejecuta el servidor con `memcheck`.
- `make valgrind_client`: Ejecuta el cliente con `memcheck`.
- `make log_server`: Ejecuta el servidor y redirige la salida a un archivo `.log`.
- `make log_client`: Ejecuta el cliente y redirige la salida a un archivo `.log`.

También es posible ejecutar los programas manualmente desde el directorio principal del proyecto:

- **Servidor:** `./bin/server <nombre>.conf`
- **Cliente:** `./bin/client IP`

## Uso de cliente
Para el cliente se seguirán los siguientes pasos en cada ejecución:
- Se indicará el verbo a usar
- Se indicará el archivo y en el caso de GET de scripts se añadirán los argumentos con un ? ejemplo: /scripts/test.py?var1=4&var2=9
- En el caso de POST en un script se tedndrá que añadir los argumentos con un formato similar al anterior: num1=5&num2=9
En el fichero client.c se podrá encontrar una mejor especificación de ejecución
