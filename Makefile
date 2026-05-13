CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11 -D_GNU_SOURCE

BIN_DIR = bin

COMMON_SRC = src/common/config.c src/common/users.c src/common/protocol.c src/common/log.c
SERVER_SRC = src/server/main.c src/server/worker.c src/server/daemon.c $(COMMON_SRC)
CLIENT_SRC = src/client/main.c src/common/protocol.c

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