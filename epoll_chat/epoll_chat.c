/*
 ============================================================================
 Name        : EPOLL_client.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include <sys/epoll.h>
#include <sys/types.h>
#include <unistd.h>

#define SERVER_PORT 12000
#define BUF_SIZE 20
#define CLIENT_ACTION 2

void error_msg(char *str){
	printf("Server error : %s \n", str);
	exit(1);
}


int main(void) {

	char buffer[BUF_SIZE];
	struct sockaddr_in socket_addr;
	int fd, ef_fd;

	//epoll variable...
	struct epoll_event ep_ev;
	struct epoll_event ep_ev_arr[CLIENT_ACTION];


	//client socket init
	fd = socket(AF_INET, SOCK_STREAM, 0);

	memset(&socket_addr, 0x00, sizeof(socket_addr));
	socket_addr.sin_family = AF_INET;
	socket_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	socket_addr.sin_port = htons(SERVER_PORT);

	//connect
	if(connect(fd, (struct sockaddr *) &socket_addr, sizeof(socket_addr)) < 0){
		error_msg("connect error!!");
	}

	//epoll setting... two setting, 1-consol, 2-connect fd
	if((ef_fd = epoll_create(1)) < 0){
		error_msg("Epoll_creat error!!");
	}

	memset(&ep_ev, 0x00, sizeof(ep_ev));
	ep_ev.events = EPOLLIN;
	ep_ev.data.fd = STDIN_FILENO;
	epoll_ctl(ef_fd, EPOLL_CTL_ADD, STDIN_FILENO, &ep_ev);

	memset(&ep_ev, 0x00, sizeof(ep_ev));
	ep_ev.events = EPOLLIN;
	ep_ev.data.fd = fd;
	epoll_ctl(ef_fd, EPOLL_CTL_ADD, fd, &ep_ev);


	while(1){
		int n_fd = epoll_wait(ef_fd, ep_ev_arr, CLIENT_ACTION, -1);

		for(int i = 0; i< n_fd; i++){
			// input descriptor, client --> server, write
			if(ep_ev_arr[i].data.fd == STDIN_FILENO){

				fgets(buffer, BUF_SIZE, stdin);
				if(!strcmp(buffer, "q\n") | !strcmp(buffer, "Q\n")){

					shutdown(fd, SHUT_WR);
					exit(0);
				}
				write(fd, buffer, strlen(buffer));
				//epoll_ctl(ef_fd, EPOLL_CTL_DEL, fd, &ep_ev);


			}
			// server --> client, show message, read
			if(ep_ev_arr[i].data.fd == fd){
				int n = read(fd, buffer, sizeof(buffer));
				if(n == 0) continue;
				buffer[n] = 0;

				printf("Message from Server : %s", buffer);

			}
		}
	}

	close(fd);
	return 0;
}
