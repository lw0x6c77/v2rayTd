#include <sys/un.h>
#include "core.h"

void wait_cli()
{
    int                 sockfd;
    struct sockaddr_un  servaddr, cliaddr;
    socklen_t	        addrlen;
	char		        recvline[MAXLINE + 1];
    char                msg[MAXLINE + 1];
    ssize_t		        n, nmsg;

    sockfd = socket(AF_LOCAL, SOCK_DGRAM, 0);
    if(sockfd < 0) {
        err_sys("can't create socket");
        exit(1);
    }

    unlink(V2RAYTD_LOCAL);
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sun_family = AF_LOCAL;
    strcpy(servaddr.sun_path, V2RAYTD_LOCAL);

    if(bind(sockfd, (SA *)&servaddr, sizeof(servaddr)) < 0) {
        err_sys("can't bind socket");
        exit(1);
    }

    for ( ; ; ) {
recv_again:
        addrlen = sizeof(cliaddr);
        n = recvfrom(sockfd, recvline, MAXLINE, 0, (struct sockaddr *)&cliaddr, &addrlen);
		if (n < 0) {
            if (errno == EINTR) {
                err_msg("recv interrupt, do again");
                goto recv_again;
            } else {
                err_sys("recvfrom error");
                exit(1);
            }
        } else if(n == 0) {
            err_msg("message is empty");
            continue;
        }

        nmsg = MAXLINE;
        recvline[n] = '\0';
        parse_cmd(recvline, n, msg, &nmsg);
        if (strlen(msg) == 0 || nmsg == 0) {
            err_msg("invalid response");
            continue;
        }

send_again:
        n = sendto(sockfd, msg, nmsg, 0, (SA *)&cliaddr, addrlen);
        if(n < 0) {
            if (errno == EINTR) {
                err_msg("recv interrupt, do again");
                goto send_again;
            } else {
                err_sys("sendto error");
                exit(1);
            }
        } else if (n != nmsg)
        {
            err_sys("sendto error");
            exit(1);
        }
	}
}
