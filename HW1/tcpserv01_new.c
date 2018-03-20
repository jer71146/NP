#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<time.h>
#include"unp.h"

int main(int argc, char **argv)
{
	int					listenfd, connfd, sockfd;
	int 				i, maxi, maxfd;
	int 				nready, client[FD_SETSIZE];
	int 				cli_port;
	struct client_info
	{
		int 			cli[FD_SETSIZE];
		int 			index[FD_SETSIZE];
		char 			ip[FD_SETSIZE][16];
		int  			cli_port[FD_SETSIZE];
		char 			name[FD_SETSIZE][15];
	} cc;
	ssize_t 			n;
	fd_set				rset, allset;
	char				tmp[MAXLINE], line[MAXLINE], str[MAXLINE], say_hi[MAXLINE];
	char				w_line[MAXLINE], w1_line[MAXLINE];
	char				n_line[MAXLINE], n1_line[MAXLINE];
	char				t_line[MAXLINE], t1_line[MAXLINE];
	char				e_line[MAXLINE], e1_line[MAXLINE], e2_line[MAXLINE];
	char				err_line[MAXLINE], err_line_1[MAXLINE];
	socklen_t			clilen;
	struct sockaddr_in	cliaddr, servaddr;

	listenfd = socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(1093);

	bind(listenfd, (SA *) &servaddr, sizeof(servaddr));

	listen(listenfd, LISTENQ);

	maxfd = listenfd;
	maxi = -1;
	//for(i = 0;i < FD_SETSIZE; i++)
	//	client[i] = -1;
	for(i = 0;i < FD_SETSIZE; i++)
	{
		cc.cli[i] = -1;
		cc.index[i] = -1;
		strcpy(cc.name[i], "anonymous");
	}

	FD_ZERO(&allset);
	FD_SET(listenfd,&allset);

	int start = 0, nameit = -1;
	
	for ( ; ; ) {
		rset = allset;
		nready = select(maxfd+1, &rset, NULL, NULL, NULL);
		if(FD_ISSET(listenfd, &rset))
		{
			clilen = sizeof(cliaddr);
			connfd = accept(listenfd, (SA *) &cliaddr, &clilen);
			//printf("This is connfd %d\n", connfd);
			//printf("New client: %s, port: %d\n", inet_ntop(AF_INET, &cliaddr.sin_addr, str, INET_ADDRSTRLEN), ntohs(cliaddr.sin_port));
			inet_ntop(AF_INET, &cliaddr.sin_addr, str, INET_ADDRSTRLEN);
			cli_port = ntohs(cliaddr.sin_port);
			//printf("str: %s, cli_port: %d\n", str, cli_port);
			for(i = 0;i < FD_SETSIZE;i++)
			{
				if(cc.cli[i] < 0)
				{
					//tell old client
					say_hi[0] = '\0';
					for(int j = 0;j <= maxi;j++)
					{
						if(cc.cli[j] != -1)
						{
							say_hi[0] = '\0';
							int a = sprintf(say_hi, "[Server] Someone is coming!, maxi: %d\n", maxi);
							writen(cc.cli[j], say_hi, a);
							//clean
							say_hi[0] = '\0';
							//memset(say_hi, 0, sizeof say_hi);
						}
					}
					for(int j = 0;j < FD_SETSIZE;j++)
					{
						if(cc.cli[j] == -1)
						{
							cc.index[i] = j;
							break;
						}
					}
					cc.cli[i] = connfd;
					cc.cli_port[i] = cli_port;
					strcpy(cc.ip[i], str);
					printf("str: %s, cli_port: %d\n", cc.ip[i], cc.cli_port[i]);
					printf("number i: %d, connfd is: %d, name: %s\n",i, cc.cli[i], cc.name[i]);
					//tell new client
					int b = sprintf(say_hi ,"[Server] Hello, %s! From: %s/%d\n", cc.name[i], str, cc.cli_port[i]);
					writen(cc.cli[i], say_hi, b);
					say_hi[0] = '\0';
					break;
				}
			}
			if(i == FD_SETSIZE)
			{
				printf("Too many clients.");
				break;
			}
			FD_SET(connfd, &allset);
			if(connfd > maxfd)
				maxfd = connfd;
			if(i > maxi)
				maxi = i;
			if(--nready <= 0)
				continue;
		}
		int offline_num = 0, n1_exit;
		for(i = 0;i <= maxi;i++)
		{
			if((sockfd = cc.cli[i]) < 0)
				continue;
			if(FD_ISSET(sockfd, & rset))
			{
				if((n = readline(sockfd, tmp, MAXLINE)) == 0)
				{
					//printf("close client i: %d\n", i);
					//find which is empty
					for(int j = 0;j <= maxi;j++)
					{
						if(i != j)
						{
							n1_exit = sprintf(e1_line ,"[Server] %s is offline.\n", cc.name[i]);
							printf("e1_line: %s", e1_line);
							writen(cc.cli[j], e1_line, n1_exit);
						}
					}
					strcpy(cc.name[i], "anonymous");
					cc.ip[i][0] = '\0';
					cc.cli[i] = -1;
					cc.cli_port[i] = 0;
					start++;
					close(sockfd);
					FD_CLR(sockfd, &allset);
				}
				else
				{
					int ind = 0, msg_2_ind = 0, msg_3_ind = 0;
					char msg_1[20], msg_2[20], msg_3[20];
					char last[20];
					msg_1[0] = '\0';
					printf("tmp_len: %d, tmp: %s\n", strlen(tmp), tmp);
					while(tmp[ind] != ' ' && ind < strlen(tmp)-1)
					{
						msg_1[ind] = tmp[ind];
						ind++;
					}
					//printf("tmp_len: %d, ind: %d\n", strlen(tmp), ind);
					//for(int j = 0; j <= strlen(msg_1); j++)
                	//	if(msg_1[j] == ' ')
                    //		msg_1[j] = '\0';
					msg_1[ind] = '\0';
					printf("This is msg_1: %s\n", msg_1);
					if(strcmp(msg_1, "who") == 0 || strcmp(tmp, "who\r\n") == 0)
					{
						//who
						printf("maxi is : %d\n", maxi);
						int n_who, n1_who;
						for(int j = 0;j <= maxi;j++)
						{
							printf("here is who\n");
							if(cc.cli[j] != -1)
							{
								if(cc.cli[j] == sockfd)
								{
									printf("w+ cc.cli: %d, sockfd: %d\n", cc.cli[j], sockfd);
									n_who = sprintf(w_line ,"[Server] %s %s/%d %d->me\n", cc.name[j], cc.ip[j], cc.cli_port[j], cc.cli[j]);
									write(sockfd, w_line, n_who);
									printf("w says: %s", w_line);
								}
								else //if(cc.cli_port[j] != 0)
								{
									printf("w1+ cc.cli: %d, sockfd: %d\n", cc.cli[j], sockfd);
									n1_who = sprintf(w1_line ,"[Server] %s %s/%d %d\n", cc.name[j], cc.ip[j], cc.cli_port[j], cc.cli[j]);
									write(sockfd, w1_line, n1_who);
									printf("w1 says: %s", w1_line);
								}
							}
						}
					}
					else if(strcmp(msg_1, "tell") == 0)
					{
						//tell
						//get msg_2
						ind++;
						msg_2_ind = 0;
						msg_2[0] = '\0';
						while(tmp[ind] != ' ' && ind < strlen(tmp)-1)
						{
							msg_2[msg_2_ind] = tmp[ind];
							ind++;
							msg_2_ind++;
							//printf("index: %d, char: %c\n", msg_2_ind, msg_2[msg_2_ind]);
						}
						msg_2[msg_2_ind] = '\0';
						//get msg_3
						ind++;
						int m2_empty = 0;
						for(int j = 0;j < strlen(msg_2);j++)
						{
							if(msg_2[j] - ' ' == 0)
								m2_empty = 1;
						}
						if( (strlen(msg_2) == 0) || m2_empty)
						{
							int t_m2_error;
							t_m2_error = sprintf(err_line ,"[Server] ERROR: Error command\n");
							printf("tell<space><space>\n");
							writen(sockfd, err_line, t_m2_error);
							continue;	
						}
						msg_3_ind = 0;
						msg_3[0] = '\0';
						while(ind < strlen(tmp)-1 && tmp[ind] != '\r')
						{
							msg_3[msg_3_ind] = tmp[ind];
							ind++;
							msg_3_ind++;
							//printf("index: %d, char: %c\n", msg_2_ind, msg_2[msg_2_ind]);
						}
						msg_3[msg_3_ind] = '\0';
						if( (strlen(msg_3) == 0) || (msg_3[0] - ' ' == 0))
						{
							int t_m3_error;
							t_m3_error = sprintf(err_line ,"[Server] ERROR: Error command\n");
							printf("tell<space>someone<space>\n");
							writen(sockfd, err_line, t_m3_error);
							continue;	
						}
						//for(int j = ind; j < strlen(tmp); j++)

						printf("TELL=> msg_2: %s, msg_3: %s\n", msg_2, msg_3);
						int r_anony = 0, s_anony = 0, find_recv = 0, corr_receiver = -1;
						int t_name, t1_name;
						
						if(strcmp(msg_2, "anonymous") == 0)
						{
							r_anony = 1;
							printf("receiver is anonymous\n");
						}
						if(strcmp(cc.name[cc.index[i]], "anonymous") == 0)
						{
							s_anony = 1;
							printf("sender is anonymous\n");
						}
						for(int j = 0; j <= maxi; j++)
						{
							if(strcmp(msg_2, cc.name[j]) == 0)
							{
								find_recv = 1;
								corr_receiver = j;
							}
						}
						printf("r_anony: %d, s_anony: %d, find_recv: %d\n", r_anony, s_anony, find_recv);
						if(s_anony)
						{
							t_name = sprintf(t_line ,"[Server] ERROR: You are anonymous.\n");
							writen(sockfd, t_line, t_name);
						}
						else if(r_anony)
						{
							t_name = sprintf(t_line ,"[Server] ERROR: The client to which you sent is anonymous.\n");
							writen(sockfd, t_line, t_name);
						}
						else if(find_recv == 0)
						{
							t_name = sprintf(t_line ,"[Server] ERROR: The receiver doesn't exist.\n");
							writen(sockfd, t_line, t_name);
						}
						else if(find_recv)
						{
							//to sender
							t_name = sprintf(t_line ,"[Server] SUCCESS: Your message has been sent.\n");
							writen(sockfd, t_line, t_name);
							//to receiver
							t1_name = sprintf(t1_line ,"[Server] %s tell you %s.\n", cc.name[cc.index[i]], msg_3);
							writen(cc.cli[corr_receiver], t1_line, t1_name);
						}
						//printf("here is tell\n");

					}
					else if(strcmp(msg_1, "name") == 0)
					{
						//name
						ind++;
						msg_2_ind = 0;
						msg_2[0] = '\0';
						while(ind < strlen(tmp)-1 && tmp[ind] != '\r')
						{
							msg_2[msg_2_ind] = tmp[ind];
							ind++;
							msg_2_ind++;
							//printf("index: %d, char: %c\n", msg_2_ind, msg_2[msg_2_ind]);
						}
						msg_2[msg_2_ind] = '\0';
						//printf("msg_2_ind: %d\n", msg_2_ind);
						//printf("This is msg_2: %s, length: %d\n", msg_2, strlen(msg_2));
						//printf("This is tmp: %s, length: %d\n", tmp, strlen(tmp));
						printf("here is name\n");
						int not_eng_let = 0, err_len = 0, anony = 0, used = 0;
						for(int j = 0;j < strlen(msg_2);j++)
						{
							if( ! ( (msg_2[j] - 'a' >= 0 && msg_2[j] - 'z' <= 0) || (msg_2[j] - 'A' >= 0 && msg_2[j] - 'Z' <= 0) ) )
								not_eng_let = 1;
						}
						if(strlen(msg_2) > 12 || strlen(msg_2) < 2)
							err_len = 1;

						if(strcmp(msg_2, "anonymous") == 0)
						{
							anony = 1;
							printf("here is anonymous\n");
						}
						for(int j = 0;j <= maxi;j++)
						{
							if(strcmp(msg_2, cc.name[j]) == 0)
								used = 1;
						}
						printf("not_eng_let: %d, err_len: %d, anony: %d, used: %d\n", not_eng_let, err_len, anony, used);
						int n_name, n1_name;
						int namer;
						printf("Here is cc.cli\n");
						for(int j = 0; j <= maxi; j++)
						{
							printf("cc.cli[%d]: %d\n", j, cc.cli[j]);
						}
						printf("\n\n");
						printf("namer: %d\n", cc.index[i]);

						char old_name[50];
						strcpy(old_name, cc.name[namer]);
						if(not_eng_let || err_len)
						{
							n_name = sprintf(n_line ,"[Server] ERROR: Username can only consists of 2~12 English letters.\n");
							writen(sockfd, n_line, n_name);
						}
						else if(anony)
						{
							n_name = sprintf(n_line ,"[Server] ERROR: Username can not be anonymous\n");
							writen(sockfd, n_line, n_name);
						}
						else if(used)
						{
							n_name = sprintf(n_line ,"[Server] ERROR: %s has been used by others.\n", msg_2);
							writen(sockfd, n_line, n_name);
						}
						else
						{
							strcpy(cc.name[cc.index[i]], msg_2);
							//printf("cc.name[%d]: %s, length: %d\n", namer, cc.name[namer], strlen(cc.name[namer]));
							for(int j = 0;j <= maxi;j++)
							{
								if(cc.cli[j] == sockfd)
								{
									n_name = sprintf(n_line ,"[Server] You're now known as %s.\n", cc.name[cc.index[i]]);
									printf("e_line: %s\n", n_line);
									writen(sockfd, n_line, n_name);
								}
								else
								{
									n1_name = sprintf(n1_line ,"[Server] %s is now known as %s.\n", old_name, cc.name[cc.index[i]]);
									printf("e1_line: %s\n", n1_line);
									writen(cc.cli[j], n1_line, n1_name);
								}
							}
						}
					}
					else if(strcmp(msg_1, "yell") == 0)
					{
						//yell
						ind++;
						msg_2_ind = 0;
						msg_2[0] = '\0';
						while(ind < strlen(tmp)-1 && tmp[ind] != '\r')
						{
							msg_2[msg_2_ind] = tmp[ind];
							ind++;
							msg_2_ind++;
						}
						msg_2[msg_2_ind] = '\0';
						if( (strlen(msg_2) == 0) || (msg_2[0] - ' ' == 0))
						{
							int y_m2_error;
							y_m2_error = sprintf(err_line ,"[Server] ERROR: Error command\n");
							printf("yell<space>\n");
							writen(sockfd, err_line, y_m2_error);
							continue;	
						}
                    	printf("This is msg_2: %s\n", msg_2);
						printf("here is yell\n");
						for(int j = 0;j <= maxi;j++)
						{
							printf("cc.name[%d]: %s\n", j, cc.name[j]);
							printf("sockfd: %d\n", sockfd);
							int n = sprintf(line ,"[Server] %s yell %s\n", cc.name[cc.index[i]], msg_2);
							printf("line: %s", line);
							writen(cc.cli[j], line, n);
						}
					}
					else if(strcmp(msg_1, "exit") == 0 || strcmp(tmp, "exit\r\n") == 0)
					{
						//exit
						for(int j = 0;j <= maxi;j++)
						{
							if(i != j)
							{
								int e2_exit = sprintf(e2_line ,"[Server] %s is offline.\n", cc.name[i]);
								printf("e2_line: %s", e2_line);
								writen(cc.cli[j], e2_line, e2_exit);
							}
						}
						if(strcmp(tmp, "exit\r\n") == 0)
						{
							printf("GET R \n");
							close(connfd);
						}
						printf("here is exit\n");
						int n_exit;
						n_exit = sprintf(e_line ,"bye\n");
						writen(sockfd, e_line, n_exit);
						strcpy(cc.name[i], "anonymous");
						cc.ip[i][0] = '\0';
						cc.cli[i] = -1;
						cc.cli_port[i] = 0;
						start++;
						close(sockfd);
						FD_CLR(sockfd, &allset);
						//find which is empty
					
					}
					else 
					{
						//else
						int error;
						error = sprintf(err_line ,"[Server] ERROR: Error command\n");
						writen(sockfd, err_line, error);
					}	
					/*printf("ind: %d, tmp: %d\n", ind, strlen(tmp));
					if(ind != strlen(tmp))
					{
						int error_1;
						error_1 = sprintf(err_line_1 ,"[Server] ERROR: Error command\n");
						writen(sockfd, err_line_1, error_1);
					}*/
				}
				if(--nready <= 0)
					break;
					
			}

		}
		//if ( (childpid = fork()) == 0) {	/* child process */
		//	close(listenfd);	/* close listening socket */
		//	str_echo(connfd);	/* process the request */
		//	exit(0);
		//}
		//close(connfd);			/* parent closes connected socket */
	}
}
void
str_echo(int sockfd)
{
	long		arg1, arg2;
	ssize_t		n;
	char		line[MAXLINE];

	for ( ; ; ) {
		if ( (n = readline(sockfd, line, MAXLINE)) == 0)
			return;		/* connection closed by other end */

		if (sscanf(line, "%ld%ld", &arg1, &arg2) == 2)
			snprintf(line, sizeof(line), "%ld\n", arg1 + arg2);
		else
			snprintf(line, sizeof(line), "input error\n");

		n = strlen(line);
		//sleep(10);
		writen(sockfd, line, n);
	}
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
