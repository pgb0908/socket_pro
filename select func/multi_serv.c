#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>

#define BUF_LEN 100

int main(int argc,char* argv[]){

	int serv_fd, cli_fd;
    struct sockaddr_in serv_addr, cli_addr; 
    struct timeval timeout;
    fd_set reads, cpy_reads;
	
    socklen_t adr_size;
	int fd_max, str_len, fd_num, i;
    char buffer[BUF_LEN];
    
    if(argc != 2)
    {
        printf("check:%s [port]\n", argv[0]);
        exit(1);
    }

    //create server socket
    serv_fd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, 0x00, sizeof(serv_addr));
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_port = htons(atoi(argv[1]));
     serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
     //inet_pton(AF_INET, argv[1], &serv_addr.sin_addr);

    //bind
    if(bind(serv_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
    {
        printf("server: can't bind local address. \n");
        exit(0);
    }
    
    //listen
    if(listen(serv_fd, 5) == -1)
    {
        printf("server: listen error. \n");
        exit(0);
    }
    
    FD_ZERO(&reads);
    FD_SET(serv_fd, &reads);
    fd_max = serv_fd;
    
    while(1){
        
        cpy_reads = reads;
        timeout.tv_sec=5;
        timeout.tv_usec = 5000;
        
        if((fd_num = select(fd_max+1, &cpy_reads, 0, 0, &timeout)) == -1)
        {
            break;
        }
        
        if(fd_num == 0) continue;
        
        
        for(i = 0; i < fd_max+1; i++)
        {
            if(FD_ISSET(i, &cpy_reads))
            {
                if(i == serv_fd)    // connection request!!
                {
                    adr_size = sizeof(cli_addr);
                    cli_fd = accept(serv_fd, (struct sockaddr *)&cli_addr, &adr_size);
                    FD_SET(cli_fd, &reads);
                    
                    if(fd_max < cli_fd)
                    {
                        fd_max = cli_fd;
                    }
                    
                    printf("Server : %d client connect!\n", cli_fd);
                    
                }
                else    // read message
                {        
                    str_len = read(i, buffer, BUF_LEN);
					
					printf("str_len : %d\n", &str_len);
                    if(str_len == 0)
                    {
                        FD_CLR(i, &reads);
                        close(i);
                        printf("Server : %s client closed. \n", i);
                    }
                    else
                    {
                        write(i, buffer, str_len);
                    }
                }
            }
        }
    }
    close(serv_fd);
    printf("server is dead! \n");    

    return 0;
}
