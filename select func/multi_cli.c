#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>
#include <netdb.h>

#define BUF_LEN 100

void read_routine(int sock, char *buf)
{
    int str_len=read(sock, buf, BUF_LEN);
	printf("client str_len : %d", &str_len);
    if(str_len==0)
        return;
 
    buf[str_len]=0;
    printf("Message from server: %s", buf);
}

void write_routine(int sock, char *buf)
{
    fgets(buf, BUF_LEN, stdin);
    if(!strcmp(buf,"q\n") || !strcmp(buf,"Q\n"))
    {
        shutdown(sock, SHUT_WR);
        exit(0);
    }
    write(sock, buf, strlen(buf));
}


int main(int argc, char* argv[]){
	
	int fd;
	char buffer[BUF_LEN];
    struct sockaddr_in serv_addr;
	struct timeval timeout;
	
	fd_set reads, cpy_reads;
	
	int fd_max, fd_num, rt;

    //domain name
    struct hostent *host_entry;
    int host_ip=0;

    if(argc != 3){
        printf("check : %s [ip] and %s [port] \n", argv[1], argv[2]);
        exit(1);
    }

    fd = socket(AF_INET, SOCK_STREAM, 0);
	memset(&serv_addr, 0x00, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(atoi(argv[2]));
	
	if(inet_addr(argv[1]) != INADDR_NONE)
	{
		serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
		printf("ip address : %s", argv[1]);
	}
	else
	{
		host_entry = gethostbyname(argv[1]); // if fail -> NULL
		if(host_entry != NULL){
			printf("host entry address : %lu \n",*((unsigned long *)(host_entry->h_addr_list[0])));
			host_ip = *((unsigned long *)(host_entry->h_addr_list[0]));
		}
		serv_addr.sin_addr.s_addr = host_ip;
	}

    if(connect(fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))  < 0){
        printf("system: connect failed \n");
        exit(1);
    }
	
   //fd_set형 변수 reads의 모든 비트를 0으로 초기화
    FD_ZERO(&reads);
    //입력 및 소켓 디스크립터 정보를 등록
    FD_SET(0, &reads);
    FD_SET(fd, &reads);
    //소켓의 번호를 저장
    fd_max = fd;

    while(1){
		
		cpy_reads = reads;
		
		timeout.tv_sec = 5;
		timeout.tv_usec = 0;
		
		fd_num = select(fd_max+1, &cpy_reads, 0, 0, &timeout);
        
		if(fd_num == -1)
		{
			printf("select error 발 생\n");
			exit(1);
		}
		else if(fd_num == 0)
		{
			puts("Time-out!");
			continue;
		}
		else
		{
			 //만약 입력 디스크립터에 변화가 발생했다면 (사용자로부터 데이터를 입력받았다면)
            if(FD_ISSET(0, &cpy_reads)) {
                //입력된 내용을 서버로 전송하는 함수
                write_routine(fd, buffer);
                //파일 디스크립터 정보 삭제
                FD_CLR(0, &cpy_reads);
            }
            //만약 포트에 변화가 발생했다면(서버로부터 데이터를 받았다면)
            if(FD_ISSET(fd, &cpy_reads)) {
                //받은 데이터를 출력하는 함수
                read_routine(fd, buffer);
                //파일 디스크립터 정보 삭제
                FD_CLR(fd, &cpy_reads);
            }
		}
    }
    close(fd);
    return 0;
}

