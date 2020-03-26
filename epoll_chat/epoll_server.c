/*
 ============================================================================
 Name        : EPOLL_server.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include <sys/epoll.h>
#include <sys/types.h>

#define SERVER_PORT 12000
#define BUF_SIZE 20
#define MAX_CONN 15

void error_msg(char *str){
	printf("Server error : %s \n", str);
	exit(1);
}

int main(void) {

	char buffer[BUF_SIZE];

	//socket variable...
	int server_fd, client_fd, ef_fd;
	struct sockaddr_in server_addr, client_addr;
	socklen_t adr_size;

	//epoll variable...
	struct epoll_event ep_ev;
	struct epoll_event ep_ev_arr[MAX_CONN];


	//server socket init
	server_fd = socket(AF_INET, SOCK_STREAM, 0);

	memset(&server_addr, 0x00, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);


	if(bind(server_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1){
		error_msg("Bind error!!");
	}

	//listen
	if(listen(server_fd, MAX_CONN) == -1){
		error_msg("Listen error!!");
	}

	//epoll set
	if((ef_fd = epoll_create(1)) < 0){
		error_msg("Epoll_creat error!!");
	}

	memset(&ep_ev, 0x00, sizeof(ep_ev));
	ep_ev.events = EPOLLIN;
	ep_ev.data.fd = server_fd;
	epoll_ctl(ef_fd, EPOLL_CTL_ADD, server_fd, &ep_ev);


	while(1){
		// check all fd >> n_fd
		// MAX_CONN == max events?? and wait before something to come...(-1)
	    int n_fd = epoll_wait(ef_fd, ep_ev_arr, MAX_CONN, -1);

		for(int i = 0; i< n_fd; i++){
			//server has changes --> accept client
			if(ep_ev_arr[i].data.fd == server_fd){
				adr_size = sizeof(client_addr);
				client_fd = accept(server_fd, (struct sockaddr *) &client_addr, &adr_size);

				if(client_fd < 0){
					error_msg("Accept error!!");
					continue;
				}

				//add client in ep_ev_arr[]
				memset(&ep_ev, 0x00, sizeof(ep_ev));
				ep_ev.events = EPOLLIN;
				ep_ev.data.fd = client_fd;
				epoll_ctl(ef_fd, EPOLL_CTL_ADD, client_fd, &ep_ev);
			}

			//fd is find --> read and write
			else{
				int client = ep_ev_arr[i].data.fd;
				int n = read(client, buffer, sizeof(buffer));

				if(n < 0){ //this is error

					error_msg("read error!!");
					epoll_ctl(ef_fd, EPOLL_CTL_DEL, client, &ep_ev);
					close(client);

				}else if(n == 0){ // no message

					epoll_ctl(ef_fd, EPOLL_CTL_DEL, client, &ep_ev);
					close(client);

				}else{
					write(client, buffer, n);
				}

			}
		}
	}

	close(server_fd);
	printf("Server finished Process!! \n");

	return 0;
}
