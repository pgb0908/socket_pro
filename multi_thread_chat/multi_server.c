#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<pthread.h>
#include<time.h>

#define MAX_CLI 100
#define BUF_SIZE 100
#define MAX_IP 30

int cli_socks[MAX_CLI];
int cli_cnt = 0;
pthread_mutex_t mutx;

void error_handling(char *msg);
void send_msg(char* msg, int len);
void *handle_cli(void *arg);

int main(int argc, char *argv[])
{
	int serv_sock, cli_sock;
	struct sockaddr_in serv_adr, cli_adr;
	int cli_adr_size;
	pthread_t t_id;
	
	struct tm *t;
	time_t timer = time(NULL);
	t = localtime(&timer);
	
	if(argc !=2)
	{
		printf("usage : %s <port>\n", argv[0]);
		exit(1);
	}
	
	pthread_mutex_init(&mutx, NULL);
	serv_sock = socket(PF_INET, SOCK_STREAM, 0);
	
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_adr.sin_port = htons(atoi(argv[1]));
	
	if( bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1 )
	{
		error_handling("bind() error");
	}
	if( listen(serv_sock, 5) == -1 )
	{
		error_handling("listen() error");
	}
	
	while(1)
	{
		t=localtime(&timer);
		cli_adr_size = sizeof(cli_adr);
		cli_sock = accept(serv_sock, (struct sockaddr*)&cli_adr, &cli_adr_size);
		
		pthread_mutex_lock(&mutx);
		cli_socks[cli_cnt++] = cli_sock;
		pthread_mutex_unlock(&mutx);
		
		pthread_create(&t_id, NULL, handle_cli, (void*)&cli_sock);
		pthread_detach(t_id);
		printf("Connected client IP : %s ", inet_ntoa(cli_adr.sin_addr));
		printf("%d-%d-%d %d:%d\n", t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min);
		printf("  chatter (%d/100)\n", cli_cnt);
		
	}
	close(serv_sock);
	return 0;
}

void *handle_cli(void *arg)
{
	int cli_sock=*((int*)arg);
	int str_len = 0;
	int i;
	char msg[BUF_SIZE];
	
	while((str_len = read(cli_sock, msg, sizeof(msg))) != 0)
	{
		send_msg(msg, str_len);
	}
	
	pthread_mutex_lock(&mutx);
	for(i = 0; i < cli_cnt; i++)
	{
		if(cli_sock == cli_socks[i])
		{
			while(i++ < cli_cnt-1)
			{
				cli_socks[i] = cli_socks[i-1];
			}
			break;
		}
	}
	
	cli_cnt--;
	pthread_mutex_unlock(&mutx);
	close(cli_sock);
	return NULL;
}

void send_msg(char* msg, int len)
{
	int i;
	pthread_mutex_lock(&mutx);
	for(i = 0; i<cli_cnt; i++)
	{
		write(cli_socks[i], msg, len);
	}
	pthread_mutex_unlock(&mutx);
}

void error_handling(char *msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}
