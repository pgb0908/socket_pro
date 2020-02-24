#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

int main(){

	struct sockaddr_in serv_addr;
	int fd;
	int conn;
	char message[100] = "";

	fd = socket(AF_INET, SOCK_STREAM, 0);
	memset(&serv_addr, '0', sizeof(serv_addr));
	//memset(message, '0', sizeof(message));

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(10000);
        serv_addr.sin_addr.s_addr = INADDR_ANY;


	bind(fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
	listen(fd, 5);



	//while(1){
	//	conn = accept(fd, (struct sockaddr *)NULL, NULL);
	//	write(conn, message, strlen(message));
	//	close(conn);
	//	
	//	sleep(1);
	//
	//}


	while(conn = accept(fd, (struct sockaddr *)NULL, NULL)) {
		int pid;
		if((pid = fork()) == 0) {
			while (recv(conn, message, 100, 0)>0) {
				printf("Message Received: %s\n", message);
				memset(message, 0, sizeof(message));
				message[0] = '\0';
			}
        		exit(0);
		}
	}

	return 0;
}
