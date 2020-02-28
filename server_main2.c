#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>


#define BUF_LEN 1024

int main(int argc,char* argv[]){

	struct sockaddr_in serv_addr, cli_addr;
	int serv_fd, cli_fd;
	char buffer[BUF_LEN];
	char temp[20];
	
	if(argc != 3){
		printf("check:%s [ip] and [port]\n", argv[0]);
		exit(0);
	}

	//create server socket
	serv_fd = socket(AF_INET, SOCK_STREAM, 0);
	memset(&serv_addr, 0x00, sizeof(serv_addr));

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(atoi(argv[2]));
        // serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
        inet_pton(AF_INET, argv[1], &serv_addr.sin_addr);

	//bind
	if(bind(serv_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		printf("server: can't bind local address. \n");
		exit(0);
	}
	
	//listen
	listen(serv_fd, 5);
	
	int len;
	int msg_size;
	len = sizeof(cli_addr);
	while(1){
		memset(buffer,0x00, sizeof(buffer));
		cli_fd = accept(serv_fd, (struct sockaddr *)&cli_addr, &len);
		if(cli_fd < 0)
		{
			printf("server: accept failed.\n");
			exit(0);
		}
		
		inet_ntop(AF_INET, &cli_addr.sin_addr.s_addr, temp, sizeof(temp));
		printf("server: %s client connected. \n", temp);
		
		while(read(cli_fd, buffer, 1024) > 0){
			printf("client %s : %s", temp, buffer);
			send(cli_fd, buffer,sizeof(buffer), 0);
		}

		close(cli_fd);
		printf("Server : %s client closed. \n", temp);
	}
	close(serv_fd);
	printf("server is dead! \n");	

	return 0;
}
