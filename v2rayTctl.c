#include <sys/un.h>
#include "core.h"

void usage()
{
    printf("v2rayTctl: connect to v2rayTd\n");
    printf("\tstart_proxy: enable system proxy\n");
    printf("\tstop_proxy: disable system proxy\n");
    printf("\treload: reload configuration, restart proxy engine\n");
    printf("\texit_all: disable proxy, stop engine, stop v2rayTd\n");
    printf("\tupdate_geosite: upgrade geosite\n");
    printf("\tupdate_geoip: upgrade geoip\n");
    printf("\tquit: exit\n");
}

int main(int argc, char *argv[])
{
    int					sockfd;
	struct sockaddr_un	servaddr, cliaddr;
    char                mesg[MAXLINE];
    char                sendline[MAXLINE];
    socklen_t           addrlen;
    ssize_t		        n;
    bool                exitall = false;

	sockfd = socket(AF_LOCAL, SOCK_DGRAM, 0);
    if(sockfd < 0) {
        err_sys("can't create socket");
        exit(1);
    }

    unlink(V2RAYTCLI_LOCAL);
    memset(&cliaddr, 0, sizeof(cliaddr));
    cliaddr.sun_family = AF_LOCAL;
    strcpy(cliaddr.sun_path, V2RAYTCLI_LOCAL);
    if(bind(sockfd, (SA *)&cliaddr, sizeof(cliaddr)) < 0) {
        err_sys("can't bind socket");
        exit(1);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sun_family = AF_LOCAL;
    strcpy(servaddr.sun_path, V2RAYTD_LOCAL);

    usage();
    printf("> ");
    fflush(stdout);
    while (fgets(sendline, MAXLINE, stdin) != NULL) {
        addrlen = sizeof(servaddr);
        n = strlen(sendline);
        if (n == 1 && sendline[0] == '\n') {
            err_msg("please input command");
            goto flush_prompt;
        } else {
            n--;
            sendline[n] = '\0';     /* remove '\n' */
        }

        if(strcmp(sendline, "quit") == 0) {
            err_msg("bye...");
            exit(0);
        } else if(strcmp(sendline, "exit_all") == 0) {
            exitall = true;
        }

        if (sendto(sockfd, sendline, n, 0, (SA *)&servaddr, addrlen) != n) {
            err_sys("sendto error");
            goto flush_prompt;
        }

        if(exitall) {
            err_msg("bye...");
            exit(0);
        }

        if ((n = recvfrom(sockfd, mesg, MAXLINE, 0, NULL, NULL)) < 0) {
            err_sys("recvfrom error");
            goto flush_prompt;
        }
        mesg[n] = '\0';
        err_msg("%s", mesg);

flush_prompt:
        printf("> ");
        fflush(stdout);
    }


    return 0;
}
