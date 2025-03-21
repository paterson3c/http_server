CC = gcc -pedantic
CFLAGS = -Wall -g
BIN = bin/
OBJ_FOLDER = obj
SRC_FOLDER = src
SOCKET_FOLDER = src/utils
CLIENT_FOLDER = src/client
TEST_FOLDER = test/
CONF_PARSER_FOLDER = src/utils
PARSER_FOLDER = src/utils
UTILS_FOLDER = src/utils
RESPONSE_FOLDER = src/utils
SERVER_FOLDER = src/server
LIB_FOLDER = lib

EXE = main client
TEST = test_conf_parser

all: clean libs $(EXE)
test: $(TEST)

.PHONY: clean
clean:
	rm -f $(OBJ_FOLDER)/*.o $(BIN)$(EXE) $(LIB_FOLDER)/*.a

clear:
	rm -f $(OBJ_FOLDER)/*.o

# Compilación de bibliotecas estáticas
libs: $(LIB_FOLDER)/libsocket.a $(LIB_FOLDER)/libhttp_parser.a $(LIB_FOLDER)/libconf_parser.a

$(LIB_FOLDER)/libsocket.a: $(OBJ_FOLDER)/socket.o
	ar rcs $@ $^

$(LIB_FOLDER)/libhttp_parser.a: $(OBJ_FOLDER)/http_parser.o
	ar rcs $@ $^

$(LIB_FOLDER)/libconf_parser.a: $(OBJ_FOLDER)/conf_parser.o
	ar rcs $@ $^

# Compilación del servidor (main)
main: $(OBJ_FOLDER)/main.o $(OBJ_FOLDER)/reactive.o $(LIB_FOLDER)/libsocket.a $(LIB_FOLDER)/libhttp_parser.a $(LIB_FOLDER)/libconf_parser.a $(OBJ_FOLDER)/utils.o $(OBJ_FOLDER)/response.o
	@echo "#---------------------------"
	@echo "# Generating $@"
	@echo "# Depends on $^"
	@echo "# Has changed $<"
	$(CC) $^ -lpthread -o $(BIN)$@

# Compilación del cliente
client: $(OBJ_FOLDER)/client.o $(LIB_FOLDER)/libsocket.a
	@echo "#---------------------------"
	@echo "# Generating $@"
	@echo "# Depends on $^"
	@echo "# Has changed $<"
	$(CC) $^ -o $(BIN)$@

# Reglas de compilación de objetos individuales
$(OBJ_FOLDER)/main.o:
	$(CC) $(CFLAGS) -c $(SRC_FOLDER)/main.c -o $@

$(OBJ_FOLDER)/reactive.o:
	$(CC) $(CFLAGS) -c $(SERVER_FOLDER)/reactive.c -o $@

$(OBJ_FOLDER)/socket.o:
	$(CC) $(CFLAGS) -c $(SOCKET_FOLDER)/socket.c -o $@

$(OBJ_FOLDER)/client.o:
	$(CC) $(CFLAGS) -c $(CLIENT_FOLDER)/client.c -o $@

$(OBJ_FOLDER)/conf_parser.o:
	$(CC) $(CFLAGS) -c $(CONF_PARSER_FOLDER)/conf_parser.c -o $@

$(OBJ_FOLDER)/http_parser.o:
	$(CC) $(CFLAGS) -c $(PARSER_FOLDER)/http_parser.c -o $@

$(OBJ_FOLDER)/utils.o:
	$(CC) $(CFLAGS) -c $(UTILS_FOLDER)/utils.c -o $@

$(OBJ_FOLDER)/response.o:
	$(CC) $(CFLAGS) -c $(RESPONSE_FOLDER)/response.c -o $@

test_conf_parser: $(OBJ_FOLDER)/test_conf_parser.o $(CONF_PARSER_FOLDER)/libconf_parser.a
	@echo "#---------------------------"
	@echo "# Generating $@"
	@echo "# Depends on $^"
	@echo "# Has changed $<"
	$(CC) $^ -o $(BIN)$@

$(OBJ_FOLDER)/test_conf_parser.o:
	$(CC) $(CFLAGS) -c $(TEST_FOLDER)/test_conf_parser.c -o $@

# Ejecución del servidor
run_server:
	./bin/main ./conf/re_server.conf

# Ejecución del cliente
run_client:
	./bin/client

# Valgrind para el servidor
valgrind_server:
	valgrind --leak-check=full --show-leak-kinds=all ./bin/main ./conf/re_server.conf

# Valgrind para el cliente
valgrind_client:
	valgrind --leak-check=full --show-leak-kinds=all ./bin/client

# Log de Valgrind para el servidor
log_server:
	./bin/main ./conf/re_server.conf > log.log

# Log de Valgrind para el cliente
log_client:
	./bin/client > log_client.log