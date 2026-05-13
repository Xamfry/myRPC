#include "../common/config.h"
#include "../common/protocol.h"
#include "../common/users.h"
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#define DEFAULT_CONFIG_PATH "config/myRPC.conf"
#define DEFAULT_USERS_PATH "config/users.conf"
#define BACKLOG 10

static void handle_sigchld(int signo)
{
    (void)signo;

    while (waitpid(-1, NULL, WNOHANG) > 0)
    {
    }
}

static int handle_client(int client_fd, const struct user_list *users)
{
    char request_buffer[REQUEST_SIZE];
    char response_buffer[RESPONSE_SIZE];
    struct rpc_request request;
    ssize_t bytes_read;

    memset(request_buffer, 0, sizeof(request_buffer));
    memset(response_buffer, 0, sizeof(response_buffer));

    bytes_read = recv(client_fd, request_buffer, sizeof(request_buffer) - 1, 0);
    if (bytes_read <= 0)
    {
        perror("recv");
        return 1;
    }

    request_buffer[bytes_read] = '\0';

    if (parse_request(request_buffer, &request) != 0)
    {
        build_response(response_buffer, sizeof(response_buffer), 1,
                       "invalid request format");
        send(client_fd, response_buffer, strlen(response_buffer), 0);
        return 1;
    }

    printf("worker pid %d: request from user %s\n", getpid(), request.login);
    printf("worker pid %d: command %s\n", getpid(), request.command);

    if (!is_user_allowed(users, request.login))
    {
        build_response(response_buffer, sizeof(response_buffer), 1,
                       "user is not allowed");
        send(client_fd, response_buffer, strlen(response_buffer), 0);
        return 1;
    }

    build_response(response_buffer, sizeof(response_buffer), 0,
                   "request accepted by worker process");
    send(client_fd, response_buffer, strlen(response_buffer), 0);

    return 0;
}

static int run_stream_server(int port, const struct user_list *users)
{
    int server_fd;
    int client_fd;
    int opt;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    socklen_t client_len;
    struct sigaction sa;

    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handle_sigchld;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

    if (sigaction(SIGCHLD, &sa, NULL) < 0)
    {
        perror("sigaction");
        return 1;
    }

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
    {
        perror("socket");
        return 1;
    }

    opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        perror("setsockopt");
        close(server_fd);
        return 1;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons((uint16_t)port);

    if (bind(server_fd, (struct sockaddr *)&server_addr,
             sizeof(server_addr)) < 0)
    {
        perror("bind");
        close(server_fd);
        return 1;
    }

    if (listen(server_fd, BACKLOG) < 0)
    {
        perror("listen");
        close(server_fd);
        return 1;
    }

    printf("myRPC-server parent pid %d listening on port %d\n",
           getpid(), port);

    while (1)
    {
        pid_t pid;

        client_len = sizeof(client_addr);

        client_fd = accept(server_fd, (struct sockaddr *)&client_addr,
                           &client_len);
        if (client_fd < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }

            perror("accept");
            continue;
        }

        printf("client connected: %s\n", inet_ntoa(client_addr.sin_addr));

        pid = fork();
        if (pid < 0)
        {
            perror("fork");
            close(client_fd);
            continue;
        }

        if (pid == 0)
        {
            close(server_fd);
            handle_client(client_fd, users);
            close(client_fd);
            exit(0);
        }

        close(client_fd);
    }

    close(server_fd);
    return 0;
}

int main(void)
{
    struct server_config config;
    struct user_list users;

    if (load_server_config(DEFAULT_CONFIG_PATH, &config) != 0)
    {
        fprintf(stderr, "Failed to load server config\n");
        return 1;
    }

    if (load_user_list(DEFAULT_USERS_PATH, &users) != 0)
    {
        fprintf(stderr, "Failed to load users whitelist\n");
        return 1;
    }

    if (config.socket_type != SOCKET_TYPE_STREAM)
    {
        fprintf(stderr, "Only stream socket is implemented now\n");
        return 1;
    }

    return run_stream_server(config.port, &users);
}