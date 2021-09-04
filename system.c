#include "core.h"

pid_t pid;

int call_tool(const char *arg)
{
    /*  in case wait success */
    Signal(SIGCHLD, SIG_DFL);

    /* sub procss */
    if((pid = fork()) < 0) {
        err_sys("fork error");
        exit(1);
    } else if(pid == 0) {
        char path[PATH_MAX + 32];
        snprintf(path, sizeof(path), "%s/%s", g_cwd, TOOL_FILE);

        if (strcmp(arg, "on") == 0) {
            err_msg("set system proxy: http=%s, socks=%s",
                g_cfg.http_proxy, g_cfg.socks_proxy);
            if (execl("/bin/sh", "sh", path, arg, g_cfg.http_proxy,
                g_cfg.socks_proxy, (char *)0) < 0) {
                err_sys("exec %s error", path);
                exit(1);
            }
        } else if (strcmp(arg, "off") == 0) {
            err_msg("stop system proxy");
            if (execl("/bin/sh", "sh", path, arg, (char *)0) < 0) {
                err_sys("exec %s error", path);
                exit(1);
            }
        } else if (strcmp(arg, "update_geosite") == 0) {
            err_msg("update_geosite");
            if (execl("/bin/sh", "sh", path, arg, g_cfg.http_proxy,
                g_cfg.socks_proxy, g_cfg.geosite, (char *)0) < 0) {
                err_sys("exec %s error", path);
                exit(1);
            }
        } else if (strcmp(arg, "update_geoip") == 0) {
            err_msg("update_geoip");
            if (execl("/bin/sh", "sh", path, arg, g_cfg.http_proxy,
                g_cfg.socks_proxy, g_cfg.geoip, (char *)0) < 0) {
                err_sys("exec %s error", path);
                exit(1);
            }
        } else {
            err_msg("unexpected arg: %s", arg);
            exit(1);
        }
    }

    {
        int stat_loc;
        if (wait(&stat_loc) < 0) {
            err_sys("wait error");
            exit(1);
        }
        if (WIFEXITED(stat_loc) && WEXITSTATUS(stat_loc) == 0) {
            err_msg("invoke tool successfully");
            return 0;
        } else {
            err_msg("invoke tool failed");
            return 1;
        }
    }
}
