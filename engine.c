#include "core.h"

int logfd = 0;
pid_t epid = 0;

void start_engine()
{
    /* redirect io */
    if (logfd > 0) {
        close(logfd);
    }
    if (strcmp(g_cfg.stroe_log, "1") == 0) {
        int flags = O_CREAT | O_WRONLY | O_TRUNC;
        int mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
        if((logfd = open(g_cfg.proxy_log, flags, mode)) < 0) {
            err_sys("open %s error", g_cfg.proxy_log);
            exit(1);
        }
        if(dup2(logfd, STDOUT_FILENO) < 0) {
            err_sys("dup STDOUT_FILENO error");
            exit(1);
        }
        if(dup2(logfd, STDERR_FILENO) < 0) {
            err_sys("dup STDERR_FILENO error");
            exit(1);
        }
    }

    /* sub procss */
    stop_engine();

    /* in case wait child process */
    Signal(SIGCHLD, sig_child);
    if ((epid = fork()) < 0) {
        err_sys("fork error");
        exit(1);
    } else if(epid == 0) {
        char epath[PATH_MAX + 32];
        snprintf(epath, sizeof(epath), "%s/%s", g_cwd, g_cfg.proxy_engine);
        if(execl(epath, epath, "run", (char *)0) < 0) {
            err_sys("exec %s error", epath);
            exit(1);
        }
    } else {
        err_msg("start engine [%s, pid=%d] successfully",
            g_cfg.proxy_engine, epid);
    }
}

void stop_engine()
{
    if (epid > 0) {
        /*  in case wait success */
        Signal(SIGCHLD, SIG_DFL);
        if(kill(epid, SIGKILL) < 0) {
            if(errno == ESRCH) {
                err_msg("not such process");
                return;
            } else {
                err_sys("kill pid %d error", epid);
                exit(1);
            }
        }

        int stat_loc;
        if (wait(&stat_loc) < 0) {
            err_sys("wait error");
            exit(1);
        }
        err_msg("child process [%d] terminated\n", epid);
    }
}
