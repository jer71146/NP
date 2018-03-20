#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<time.h>
#include "unp.h"

int main(int argc, char **argv)
{
	int					sockfd;
	int 				num_argv2;
	struct sockaddr_in	servaddr;

	if (argc != 3)
		printf("usage: tcpcli <IPaddress>");

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	num_argv2 = atoi(argv[2]);
	servaddr.sin_port = htons(num_argv2);
	inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

	connect(sockfd, (SA *) &servaddr, sizeof(servaddr));

	str_cli(stdin, sockfd);		/* do it all */

	exit(0);
}
void 
str_cli(FILE *fp, int sockfd)
{
	int maxfdp1, stdineof, serv_err_msg = 1;
	fd_set rset;
	char	sendline[MAXLINE], recvline[MAXLINE];
	char    cli_name[MAXLINE];

	stdineof = 0;
	FD_ZERO(&rset);
	for(;;)
	{
		if(stdineof == 0)
			FD_SET(fileno(fp), &rset);
		FD_SET(sockfd, &rset);
		maxfdp1 = max(fileno(fp), sockfd) + 1;
		select(maxfdp1, &rset, NULL, NULL, NULL);
		if(FD_ISSET(sockfd, &rset))
		{
			recvline[0] = '\0';
			memset(recvline, 0, sizeof(recvline));
			if(recv(sockfd, recvline, MAXLINE, 0) == 0)
			{
				if(stdineof == 1)
				{
					printf("HERE\n");
					return;
				}
				else if(serv_err_msg)
				{
					printf("str_cli: server terminated prematurely\n");
					serv_err_msg = 0;
				}
			}
			else
			{
				char tmp[MAXLINE];
				int index = 0;
				tmp[0] = '\0';
				while(index < strlen(recvline) -1 )//&& recvline[index] != '\r' && recvline[index] != '\n'
				{
					//if(recvline[index] == '\r')
					//	printf("GOT R \n");
					//if(recvline[index] == '\n')
					//	printf("GOT N \n");
					tmp[index] = recvline[index];
					index++;
					//printf("index: %d, char: %c\n", msg_2_ind, msg_2[msg_2_ind]);
				}
				tmp[index] = '\0';
				//printf("tmp: %s, length:%d \n", tmp, strlen(tmp));
				if((strcmp(tmp, "bye") == 0))
				{
					//printf("CLIENT BYE\n");
					exit(0);
				}
				else
				{
					//printf("FINAL");
					printf("%s", recvline);
					//printf("length: %d\n", strlen(recvline));
				}
				//printf("length: %d\n", strlen(recvline));
			}
			recvline[0] = '\0';
			//fputs(recvline, stdout);
		}
		if(FD_ISSET(fileno(fp), &rset))
		{
			if(fgets(sendline, MAXLINE, fp) == NULL)
			{
				stdineof = 1;
				shutdown(sockfd, SHUT_WR);
				FD_CLR(fileno(fp), &rset);
				continue;
			}
			writen(sockfd, sendline, strlen(sendline));
		}
	}
	/*while (fgets(sendline, MAXLINE, fp) != NULL) {

		writen(sockfd, sendline, strlen(sendline));

		if (readline(sockfd, recvline, MAXLINE) == 0)
			"str_cli: server terminated prematurely");

		fputs(recvline, stdout);
	}*/
}
ssize_t						/* Write "n" bytes to a descriptor. */
writen(int fd, const void *vptr, size_t n)
{
	size_t		nleft;
	ssize_t		nwritten;
	const char	*ptr;

	ptr = vptr;
	nleft = n;
	while (nleft > 0) {
		if ( (nwritten = write(fd, ptr, nleft)) <= 0) {
			if (nwritten < 0 && errno == EINTR)
				nwritten = 0;		/* and call write() again */
			else
				return(-1);			/* error */
		}

		nleft -= nwritten;
		ptr   += nwritten;
	}
	return(n);
}
/* end writen */

void
Writen(int fd, void *ptr, size_t nbytes)
{
	if (writen(fd, ptr, nbytes) != nbytes)
		printf("writen error");
}

static int	read_cnt;
static char	*read_ptr;
static char	read_buf[MAXLINE];

static ssize_t
my_read(int fd, char *ptr)
{

	if (read_cnt <= 0) {
again:
		if ( (read_cnt = read(fd, read_buf, sizeof(read_buf))) < 0) {
			if (errno == EINTR)
				goto again;
			return(-1);
		} else if (read_cnt == 0)
			return(0);
		read_ptr = read_buf;
	}

	read_cnt--;
	*ptr = *read_ptr++;
	return(1);
}

ssize_t
readline(int fd, void *vptr, size_t maxlen)
{
	ssize_t	n, rc;
	char	c, *ptr;

	ptr = vptr;
	for (n = 1; n < maxlen; n++) {
		if ( (rc = my_read(fd, &c)) == 1) {
			*ptr++ = c;
			if (c == '\n')
				break;	/* newline is stored, like fgets() */
		} else if (rc == 0) {
			*ptr = 0;
			return(n - 1);	/* EOF, n - 1 bytes were read */
		} else
			return(-1);		/* error, errno set by read() */
	}

	*ptr = 0;	/* null terminate like fgets() */
	return(n);
}

ssize_t
readlinebuf(void **vptrptr)
{
	if (read_cnt)
		*vptrptr = read_ptr;
	return(read_cnt);
}
/* end readline */

ssize_t
Readline(int fd, void *ptr, size_t maxlen)
{
	ssize_t		n;

	if ( (n = readline(fd, ptr, maxlen)) < 0)
		printf("readline error");
	return(n);
}
