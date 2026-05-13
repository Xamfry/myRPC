CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11

BIN_DIR = bin

COMMON_SRC = src/common/config.c
CLIENT_SRC = src/client/main.c
SERVER_SRC = src/server/main.c $(COMMON_SRC)

CLIENT_BIN = $(BIN_DIR)/myRPC-client
SERVER_BIN = $(BIN_DIR)/myRPC-server

.PHONY: all clean deb

all: $(CLIENT_BIN) $(SERVER_BIN)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(CLIENT_BIN): $(BIN_DIR) $(CLIENT_SRC)
	$(CC) $(CFLAGS) -o $(CLIENT_BIN) $(CLIENT_SRC)

$(SERVER_BIN): $(BIN_DIR) $(SERVER_SRC)
	$(CC) $(CFLAGS) -o $(SERVER_BIN) $(SERVER_SRC)

clean:
	rm -rf $(BIN_DIR)

deb:
	@echo "Deb package build will be added later"