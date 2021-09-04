#include "core.h"

void daemonize(const char *cmd)
{
    int i, fd0, fd1, fd2;
    pid_t pid;
    struct rlimit rl;

    umask(0);
    if(getrlimit(RLIMIT_NOFILE, &rl) < 0) {
        err_sys("cat't get file limit");
        exit(1);
    }

    if((pid = fork()) < 0) {
        err_msg("can't fork");
        exit(1);
    } else if(pid != 0) {
        exit(0);
    }

    openlog(cmd, LOG_CONS, LOG_DAEMON);
    daemon_proc = 1; /* output syslog */

    setsid();
    Signal(SIGHUP, SIG_IGN);

    if((pid = fork()) < 0) {
        err_msg("can't fork");
        exit(1);
    } else if(pid != 0) {
        exit(0);
    }

    // if(chdir("/") < 0) {
    //     err_msg("can't change directory to /");
    //     exit(1);
    // }

    if(rl.rlim_max == RLIM_INFINITY)
        rl.rlim_max = 1024;
    for(i = 0; i < rl.rlim_max; ++i)
        close(i);

    fd0 = open("/dev/null", O_RDWR);
    fd1 = dup(0);
    fd2 = dup(0);

    if(fd0 != 0 || fd1 != 1|| fd2 != 2) {
        err_msg("unexpected file descriptors %d %d %d", fd0, fd1, fd2);
        exit(1);
    }
}

int lockfile(int fd)
{
    struct flock fl;

    fl.l_type = F_WRLCK;
    fl.l_start = 0;
    fl.l_whence = SEEK_SET;
    fl.l_len = 0;
    return(fcntl(fd, F_SETLK, &fl));
}

int already_running()
{
    int fd;

    fd = open(LOCKFILE, O_RDWR | O_CREAT, FILEMODE);
    if(fd < 0) {
        err_sys("can't open %s", LOCKFILE);
        exit(1);
    }

    if(lockfile(fd) < 0) {
        if(errno == EACCES || errno == EAGAIN) {
            close(fd);
            return 1;
        }
        err_sys("can't lock %s", LOCKFILE);
        exit(1);
    }

    ftruncate(fd, 0);
    dprintf(fd, "%ld\n", (long)getpid());
    return 0;
}
