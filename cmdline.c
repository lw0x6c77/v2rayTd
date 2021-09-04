#include "core.h"

#define CMD_START_PROXY         "start_proxy"
#define CMD_STOP_PROXY          "stop_proxy"
#define CMD_RELOAD              "reload"
#define CMD_EXIT_ALL            "exit_all"
#define CMD_UPDATE_GEOSITE      "update_geosite"
#define CMD_UPDATE_GEOIP        "update_geoip"

int start_proxy()
{
    if (call_tool("on") == 1)
        return 1;
    return 0;
}

int stop_proxy()
{
    return call_tool("off");
}

int reload()
{
    if (!init_config()) {
        err_msg("load configuration error, exit 1");
        exit(1);
    }
    start_proxy();
    start_engine();
    return 0;
}

void exit_all()
{
    stop_proxy();
    stop_engine();
    err_msg("exit...");
}

int update_geosite()
{
    if (call_tool("update_geosite") == 1)
        return 1;
    start_engine();
    return 0;
}

int update_geoip()
{
    if (call_tool("update_geoip") == 1)
        return 1;
    start_engine();
    return 0;
}

int parse_cmd(const char *pcmd, ssize_t ncmd, char *pmsg, ssize_t *nmsg)
{
    int ret = -1;

    err_msg("recv: %s [%d bytes]", pcmd, ncmd);
    if (strcmp(pcmd, CMD_START_PROXY) == 0) {
        ret = start_proxy();
    } else if (strcmp(pcmd, CMD_STOP_PROXY) == 0) {
        ret = stop_proxy();
    } else if (strcmp(pcmd, CMD_RELOAD) == 0) {
        ret = reload();
    } else if (strcmp(pcmd, CMD_EXIT_ALL) == 0) {
        exit_all();
        exit(0);
    } else if (strcmp(pcmd, CMD_UPDATE_GEOSITE) == 0) {
        ret = update_geosite();
    } else if (strcmp(pcmd, CMD_UPDATE_GEOIP) == 0) {
        ret = update_geoip();
    } else {
        snprintf(pmsg, *nmsg, "unexpected command: %s", pcmd);
        goto len;
    }

    if (ret == 0)
        snprintf(pmsg, *nmsg, "%s", "success");
    else
        snprintf(pmsg, *nmsg, "%s", "failed");

len:
    *nmsg = strlen(pmsg);
    err_msg("resp: %s [%d bytes]", pmsg, *nmsg);
    return ret;
}
