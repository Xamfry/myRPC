#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SOCKET_STREAM 1
#define SOCKET_DGRAM 2

struct client_options
{
    char *host;
    int port;
    int socket_type;
    char *command;
};

static void print_help(const char *program_name)
{
    printf("Usage: %s [OPTIONS]\n", program_name);
    printf("\n");
    printf("Options:\n");
    printf("  -h, --host HOST        Server IP address\n");
    printf("  -p, --port PORT        Server port\n");
    printf("  -c, --command COMMAND  Bash command\n");
    printf("  -s, --stream           Use stream socket\n");
    printf("  -d, --dgram            Use datagram socket\n");
    printf("      --help             Show this help\n");
}

static int parse_arguments(int argc, char **argv, struct client_options *options)
{
    int opt;
    int option_index = 0;

    static struct option long_options[] = {
        {"host", required_argument, 0, 'h'},
        {"port", required_argument, 0, 'p'},
        {"command", required_argument, 0, 'c'},
        {"stream", no_argument, 0, 's'},
        {"dgram", no_argument, 0, 'd'},
        {"help", no_argument, 0, 1000},
        {0, 0, 0, 0}};

    options->host = NULL;
    options->port = 0;
    options->socket_type = 0;
    options->command = NULL;

    while ((opt = getopt_long(argc, argv, "h:p:c:sd", long_options,
                              &option_index)) != -1)
    {
        switch (opt)
        {
        case 'h':
            options->host = optarg;
            break;

        case 'p':
            options->port = atoi(optarg);
            break;

        case 'c':
            options->command = optarg;
            break;

        case 's':
            options->socket_type = SOCKET_STREAM;
            break;

        case 'd':
            options->socket_type = SOCKET_DGRAM;
            break;

        case 1000:
            print_help(argv[0]);
            exit(0);

        default:
            print_help(argv[0]);
            return 1;
        }
    }

    if (options->host == NULL)
    {
        fprintf(stderr, "Error: host is required\n");
        return 1;
    }

    if (options->port <= 0)
    {
        fprintf(stderr, "Error: valid port is required\n");
        return 1;
    }

    if (options->command == NULL)
    {
        fprintf(stderr, "Error: command is required\n");
        return 1;
    }

    if (options->socket_type == 0)
    {
        fprintf(stderr, "Error: socket type is required\n");
        return 1;
    }

    return 0;
}

int main(int argc, char **argv)
{
    struct client_options options;

    if (parse_arguments(argc, argv, &options) != 0)
    {
        return 1;
    }

    printf("myRPC-client options:\n");
    printf("  host: %s\n", options.host);
    printf("  port: %d\n", options.port);
    printf("  command: %s\n", options.command);

    if (options.socket_type == SOCKET_STREAM)
    {
        printf("  socket type: stream\n");
    }
    else if (options.socket_type == SOCKET_DGRAM)
    {
        printf("  socket type: dgram\n");
    }

    return 0;
}