#include "../common/config.h"
#include "../common/protocol.h"
#include "../common/users.h"
#include <stdio.h>
#define DEFAULT_CONFIG_PATH "config/myRPC.conf"
#define DEFAULT_USERS_PATH "config/users.conf"

int main(void)
{
    struct server_config config;
    struct user_list users;
    struct rpc_request request;
    char test_request[REQUEST_SIZE] = "student:whoami";

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

    print_server_config(&config);
    print_user_list(&users);

    if (parse_request(test_request, &request) != 0)
    {
        fprintf(stderr, "Failed to parse test request\n");
        return 1;
    }

    printf("parsed request:\n");
    printf("  login: %s\n", request.login);
    printf("  command: %s\n", request.command);

    if (is_user_allowed(&users, request.login))
    {
        printf("user is allowed\n");
    }
    else
    {
        printf("user is denied\n");
    }

    return 0;
}