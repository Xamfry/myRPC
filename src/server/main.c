#include "../common/config.h"
#include "../common/users.h"

#include <stdio.h>

#define DEFAULT_CONFIG_PATH "config/myRPC.conf"
#define DEFAULT_USERS_PATH "config/users.conf"

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

    print_server_config(&config);
    print_user_list(&users);

    if (is_user_allowed(&users, "student"))
    {
        printf("test: user student is allowed\n");
    }
    else
    {
        printf("test: user student is denied\n");
    }

    return 0;
}