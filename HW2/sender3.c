#include <stdio.h>
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
} p_i, p_o;


void my_dg_cli(FILE*, int, const struct sockaddr*, socklen_t, char*);

void sig_alarm(int a){
    printf("S3: Timeout!\n");
}

int main(int argc, char** argv){
    int sockfd;
    struct sockaddr_in servaddr;

    if(argc != 4)
    {
        printf("usage: ./sender1 <IPaddress> <port> <file>");
        exit(1);
    }
    signal(SIGALRM, sig_alarm);
    siginterrupt(SIGALRM, 1);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(atoi(argv[2]));
    inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    FILE* fp = fopen(argv[3], "rb");
    if(fp == NULL) 
    { 
        printf("open fail\n"); 
    }

    my_dg_cli(fp, sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr), argv[3]);
    exit(0);
}

void my_dg_cli(FILE* fp, int sockfd, const struct sockaddr* servaddr, socklen_t servlen, char* filename){
    int     n;
    int     p_counter = 0;
    char    sendline[MAXLINE], recvline[MAXLINE];
    char    over[MAXLINE];

    p_i.seq_num = 0;
	p_i.end = 0;
	p_i.size = 0;
	p_o.seq_num = 0;
	p_o.end = 0;
	p_o.size = 0;

    connect(sockfd, (struct sockaddr*) servaddr, servlen);

    p_o.end = 0;
    strcpy(p_o.data, filename);
    p_o.seq_num = 0;
    write(sockfd, &p_o, sizeof(p_o));
    ualarm(100000,100000);
    while(1)
    {
    	while(read(sockfd, &p_i, sizeof(p_i)) < 0)
    	{
	        if(errno == EINTR)
	        { 
	            printf("Filename=>Sender Timeout!\n");
	            write(sockfd, &p_o, sizeof(p_o));
	            ualarm(100000,100000);
	        }
	    }
        if(p_counter != p_i.seq_num)
        {
            //printf("Lost: Client-filename doesn't get data.\n");
            write(sockfd, &p_o, sizeof(p_o));
        }
        else
        {
            //printf("Let's go next.\n");
            break;
        }
    }
    printf("Filename=> p_counter: %d, p_i.seq_num: %d\n", p_counter,  p_i.seq_num);
    p_counter++;
    while((n = fread(sendline, 1, sizeof(sendline), fp)) > 0)
    {
        memcpy(p_o.data, sendline, n);
        p_o.seq_num++;
        p_o.size = n;
        write(sockfd, &p_o, sizeof(p_o));
        ualarm(100000,100000);
        while(1)
        {
        	while(read(sockfd, &p_i, sizeof(p_i)) < 0)
    		{
            	if(errno == EINTR)
            	{
                	printf("Transfer=>Sender Timeout!\n");
                	write(sockfd, &p_o, sizeof(p_o));
                	ualarm(100000,100000);
            	}
            }
            if(p_counter != p_i.seq_num)
                write(sockfd, &p_o, sizeof(p_o));
            else
                break;
        }
        printf("Data=> p_counter: %d, p_i.seq_num: %d\n", p_counter,  p_i.seq_num);
        p_counter++;
    }
    p_o.end = 1;
    p_o.seq_num++;
    write(sockfd, &p_o ,sizeof(p_o));
    ualarm(100000,100000);
    while(1)
    {
    	while(read(sockfd, &p_i, sizeof(p_i)) < 0)
    	{
	        if(errno == EINTR)
	        {
	            printf("Fileover=>Sender Timeout!\n");
	            write(sockfd, &p_o, sizeof(p_o));
	            ualarm(100000,100000);
	        }
	    }
	    if(p_counter != p_i.seq_num)
            write(sockfd, &p_o, sizeof(p_o));
        else
            break;
    }
    printf("FileOver=> p_counter: %d, p_i.seq_num: %d\n", p_counter,  p_i.seq_num);
}
