#include <stdio.h>
#include <sys/socket.h> //For Sockets
#include <stdlib.h>
#include <netinet/in.h> //For the AF_INET (Address Family)
#include <string.h>

int main(){

	struct sockaddr_in serv;
	int fd;
	int conn;
	char message[100] = "";

	fd = socket(AF_INET, SOCK_STREAM, 0);
	serv.sin_family = AF_INET;
	serv.sin_port = htons(10000);

	inet_pton(AF_INET, "172.17.0.2", &serv.sin_addr);
	connect(fd, (struct sockaddr *)&serv, sizeof(serv));

	while(1){
		printf("Enter a message: ");
		fgets(message, 100, stdin);
		send(fd, message, strlen(message), 0);

	}


	return 0;
}

