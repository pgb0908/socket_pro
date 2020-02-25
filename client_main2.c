#include <stdio.h>
#include <sys/socket.h> //For Sockets
#include <stdlib.h>
#include <netinet/in.h> //For the AF_INET (Address Family)
#include <string.h>

#define MSG_BUF 1024

int main(int argc, char* argv[]){

	struct sockaddr_in serv_addr;
	int fd;
	char message[MSG_BUF];

	if(argc != 2){
		printf("check : %s  [port]", argv[1]);
		exit(1);
	}

	fd = socket(AF_INET, SOCK_STREAM, 0);

	memset(&serv_addr, 0x00, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(atoi(argv[1]));
	inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

	if(connect(fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
		printf("system: connect failed \n");
		exit(1);
	}

	while(1){
		//memory initialize...
		memset(message, 0x00, MSG_BUF);
		printf("Enter a message: ");		
		fgets(message, MSG_BUF, stdin);

		if(strlen(message) <= 0 ){
			close(fd);
			return 0;
		}
		send(fd, message, strlen(message), 0);
		

		memset(message, 0x00, MSG_BUF);
		read(fd, message, MSG_BUF);
		printf("Server said: %s", message);

	}

	close(fd);

	return 0;
}

