#include <stdio.h>
#include <sys/socket.h> //For Sockets
#include <stdlib.h>
#include <netinet/in.h> //For the AF_INET (Address Family)
#include <string.h>
#include <netdb.h>

#define MSG_BUF 1024

int main(int argc, char* argv[]){

	struct sockaddr_in serv_addr;
	int fd;
	char buffer[MSG_BUF];

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

	if(connect(fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
		printf("system: connect failed \n");
		exit(1);
	}

	while(1){
		//memory initialize...
		memset(buffer, 0x00, MSG_BUF);
		printf("Enter a message: ");		
		fgets(buffer, MSG_BUF, stdin);

		if(strlen(buffer) <= 0 ){
			close(fd);
			return 0;
		}
		send(fd, buffer, sizeof(buffer), 0);
		

		memset(buffer, 0x00, MSG_BUF);
		read(fd, buffer, MSG_BUF);
		printf("Server said: %s", buffer);

	}

	close(fd);



	return 0;
}

