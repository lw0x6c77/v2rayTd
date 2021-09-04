#include "core.h"

char g_cwd[PATH_MAX];

int main(int argc, char *argv[])
{
    daemonize("v2rayTd");
    if(already_running() != 0) {
        err_msg("v2rayTd is running");
        exit(1);
    }

    if(getcwd(g_cwd, sizeof(g_cwd)) == NULL) {
        err_sys("getcwd error");
        exit(1);
    }

    // debug
    // if(!init_config()) {
    //     err_msg("load configuration error, exit 1");
    //     exit(1);
    // }

    if(reload() != 0) {
        exit(1);
    }

    wait_cli();

    return 0;
}
