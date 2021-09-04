#ifndef _CORE_H_
#define _CORE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <syslog.h>
#include <fcntl.h>
#include <signal.h>
#include <memory.h>
#include <stdbool.h>
#include <limits.h>
#include <sys/socket.h>
#include <sys/resource.h>
#include <sys/errno.h>
#include <sys/stat.h>
#include <arpa/inet.h>

#undef max
#define max(a, b) ((a) > (b)? (a): (b))

#undef min
#define min(a, b) ((a) < (b)? (a): (b))

#define	MAXLINE		            4096	/* max text line length */
#define LOCKFILE                "/tmp/v2rayTd.pid"
#define FILEMODE                (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

/* Unix domain datagram */
#define V2RAYTCLI_LOCAL         "/tmp/v2rayTcli.local"
#define	V2RAYTD_LOCAL		    "/tmp/v2rayTd.local"

#define CFG_FILE                "v2rayTd.cfg"
#define MAX_CFG_LINE            1024

#define TOOL_FILE               "tool.sh"

#define	LISTENQ		            1024	/* 2nd argument to listen() */

#define SA                      struct sockaddr

typedef void Sigfunc(int);

#ifdef __cplusplus
extern "C" {
#endif

struct config_unit {
    unsigned short cnt;
    char *http_proxy;
    char *socks_proxy;
    char *proxy_engine;
    char *proxy_log;
    char *stroe_log;
    char *geosite;
    char *geoip;
};

extern int daemon_proc;
extern struct config_unit g_cfg;
extern char g_cwd[PATH_MAX];

Sigfunc * Signal(int signo, Sigfunc *func);

void sig_child(int signo);

void daemonize(const char *cmd);

int already_running();

void wait_cli();

bool init_config();

void relase_config();

void start_engine();

void stop_engine();

int reload();

int call_tool(const char *arg);

int parse_cmd(const char *pcmd, ssize_t ncmd, char *pmsg, ssize_t *nmsg);

void err_sys(const char *fmt, ...);

void err_msg(const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif /* _CORE_H_ */
