#include <stdio.h>
#include <cstdio>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <errno.h>
#include <signal.h>
#define MAXLINE 800

struct packet_info{
    int 	end;
    int 	seq_num;
    int 	size;
    char 	data[MAXLINE];
}p_i;

void dg_recv(int, struct sockaddr*, socklen_t);
void sig_alarm(int a){
    printf("R3: Timeout!\n");
}

int main(int argc, char** argv){
    int sockfd;
    struct sockaddr_in servaddr, cliaddr;
    if(argc != 2)
    {
        printf("usage: ./receiver1 <port>");
        exit(1);
    }
    signal(SIGALRM, sig_alarm);
    siginterrupt(SIGALRM, 1);
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	int a1 = atoi(argv[1]);
	servaddr.sin_port        = htons(a1);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));

    dg_recv(sockfd, (struct sockaddr*)&cliaddr, sizeof(cliaddr));
}

void dg_recv(int sockfd, struct sockaddr* cliaddr, socklen_t clilen){
    int 		n;
    int 		now_seq_num = 0;
    int 		f_c = 1;
    socklen_t 	len;
    char 		mesg[MAXLINE];

    FILE* fpp;

    for(;;)
    {
        len = clilen;
        ualarm(100000,100000);
        while(n = recvfrom(sockfd, &p_i, sizeof(p_i), 0, cliaddr, &len) < 0)
        {
        	if(errno == EINTR)
        	{
            	p_i.seq_num = now_seq_num - 1;
            	sendto(sockfd, &p_i, sizeof(p_i), 0 ,cliaddr, len);
            	ualarm(100000,100000);
            }
        }
        if(p_i.seq_num == 0 && f_c)
        {
            now_seq_num = 0;
        }
        if(now_seq_num == p_i.seq_num)
        {  
            if(p_i.seq_num == 0)
            {
                fpp = fopen(p_i.data, "wb"); //change output_3.jpg to p_i.data
                if(fpp == NULL) 
				{ 
					printf("open error\n"); 
				}
                f_c = 0;
                now_seq_num++;
                printf("receiver open file\n");
                sendto(sockfd, &p_i, sizeof(p_i), 0, cliaddr, len);
                //printf("p_i.seq_num: %d, p_i.data: %s\n", p_i.seq_num, p_i.data);
                continue;
            }
            if(p_i.end == 1 && !f_c)
            {
                printf("Fileover: %d\n", p_i.seq_num);
                fclose(fpp);
                f_c = 1;
                //now_seq_num = 0;
                sendto(sockfd, &p_i, sizeof(p_i), 0, cliaddr, len);
                printf("File Transfer End!\n");
                continue;
            }
            printf("Writing %d, size: %d\n", p_i.seq_num, p_i.size);
            fwrite(p_i.data, 1, p_i.size, fpp);
            sendto(sockfd, &p_i, sizeof(p_i), 0, cliaddr, len);
            now_seq_num++;
        }
        else
        {
            printf("now_seq_num: %d, now_seq_num: %d\n", now_seq_num, p_i.seq_num);
            p_i.seq_num = now_seq_num - 1;
            sendto(sockfd, &p_i, sizeof(p_i), 0, cliaddr, len);
        }
    }
}
