#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "core.h"

/*
 * # simple.cfg
 * # comment
 * k1    v1
 * k2    v2    # comment
 */

#define CFG_KEY_COUNT           7
#define C_HTTP_PROXY            "http_proxy"
#define C_SOCKS_PROXY           "socks_proxy"
#define C_PROXY_ENGINE          "proxy_engine"
#define C_PROXY_LOG             "proxy_log"
#define C_STORE_LOG             "stroe_log"
#define C_GEOSITE               "geosite"
#define C_GEOIP                 "geoip"

struct config_unit g_cfg;

/*
 * parse line
 * if success return 0, error return -1, empty return 1
 */
static int split_line(const char *line, ssize_t n, char *k, char *v)
{
    int ki = 0;
    int vi = 0;
    int change = 0;
    char c;

    if(n > MAX_CFG_LINE) {
        err_msg("length gt %d, %s", MAX_CFG_LINE, line);
        return -1;
    }

    for (int i = 0; i < n; ++i) {
        if((c = line[i]) == '#') {
            break;
        }
        else if(c == '\r' || c == '\n') {
            break;
        }
        else if(c == ' ' || c == '\t') {
            if (ki > 0 && change == 0) {
                change = 1;
            }
            else if(vi > 0 && change == 1) {
                break;
            }
            continue;
        }
        else {
            if(change == 0) {
                k[ki++] = c;
            }
            else {
                v[vi++] = c;
            }
        }
    }

    if (ki > 0 && vi > 0) {
        k[ki] = '\0';
        v[vi] = '\0';
        // err_msg("%s, %s", k, v);
        return 0;
    } else if(ki == 0 && vi == 0) {
        return 1;
    }

    return -1;
}

static bool stroe_config(const char *k, const char *v)
{
    bool ret = false;
    char *p = (char *)malloc(strlen(v));
    if (!p) {
        err_sys("malloc memory error");
        goto err_clean;
    }
    strcpy(p, v);

    if (strcmp(k, C_HTTP_PROXY) == 0) {
        g_cfg.http_proxy = p;
        g_cfg.cnt += 1;
        ret = true;
    } else if (strcmp(k, C_SOCKS_PROXY) == 0) {
        g_cfg.socks_proxy = p;
        g_cfg.cnt += 1;
        ret = true;
    } else if (strcmp(k, C_PROXY_ENGINE) == 0) {
        g_cfg.proxy_engine = p;
        g_cfg.cnt += 1;
        ret = true;
    } else if (strcmp(k, C_PROXY_LOG) == 0) {
        g_cfg.proxy_log = p;
        g_cfg.cnt += 1;
        ret = true;
    } else if (strcmp(k, C_STORE_LOG) == 0) {
        g_cfg.stroe_log = p;
        g_cfg.cnt += 1;
        ret = true;
    } else if (strcmp(k, C_GEOSITE) == 0) {
        g_cfg.geosite = p;
        g_cfg.cnt += 1;
        ret = true;
    } else if (strcmp(k, C_GEOIP) == 0) {
        g_cfg.geoip = p;
        g_cfg.cnt += 1;
        ret = true;
    } else {
        err_msg("unexpected k: %s", k);
    }

err_clean:
    if (!ret && p)
        free(p);
    return ret;
}

static bool read_configuration()
{
    FILE *fp        = NULL;
    size_t n        = 0;
    ssize_t nread   = 0;
    char *line      = NULL;
    bool ret = true;
    char k[MAX_CFG_LINE + 1];
    char v[MAX_CFG_LINE + 1];

    if ((fp = fopen(CFG_FILE, "r")) == NULL) {
        err_msg("open %s error: %s\n", CFG_FILE, strerror(errno));
        ret = false;
        goto err_clean;
    }

    while((nread = getline(&line, &n, fp)) > 0) {
        int code = split_line(line, nread, k, v);
        if(code == -1) {
            ret = false;
            goto err_clean;
        } else if(code == 1) {
            continue;
        }

        if(!stroe_config(k, v)) {
            err_msg("invalid line: %s", line);
            ret = false;
            goto err_clean;
        }
    }

err_clean:
    if(line)
        free(line);

    return ret;
}

/*
 * load configuration file
 * if success return true otherwise return false
 */
bool init_config()
{
    if(g_cfg.cnt != 0)
        relase_config();
    g_cfg.http_proxy = NULL;
    g_cfg.socks_proxy = NULL;
    g_cfg.proxy_engine = NULL;
    g_cfg.proxy_log = NULL;
    g_cfg.stroe_log = NULL;
    g_cfg.geosite = NULL;
    g_cfg.geoip = NULL;
    g_cfg.cnt = 0;

    if (!read_configuration())
        return false;
    if (g_cfg.cnt != CFG_KEY_COUNT) {
        err_msg("missing configuration items");
        return false;
    }
    err_msg("load configuration file successfully");
    return true;
}

void relase_config()
{
    if (g_cfg.http_proxy)
        free(g_cfg.http_proxy);
    if (g_cfg.socks_proxy)
        free(g_cfg.socks_proxy);
    if (g_cfg.proxy_engine)
        free(g_cfg.proxy_engine);
    if (g_cfg.proxy_log)
        free(g_cfg.proxy_log);
    if (g_cfg.stroe_log)
        free(g_cfg.stroe_log);
    if (g_cfg.geosite)
        free(g_cfg.geosite);
    if (g_cfg.geoip)
        free(g_cfg.geoip);
}
