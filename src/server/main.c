#include "../common/config.h"
#include <stdio.h>
#define DEFAULT_CONFIG_PATH "config/myRPC.conf"

int main(void)
{
    struct server_config config;

    if (load_server_config(DEFAULT_CONFIG_PATH, &config) != 0)
    {
        fprintf(stderr, "Failed to load server config\n");
        return 1;
    }

    print_server_config(&config);

    return 0;
}