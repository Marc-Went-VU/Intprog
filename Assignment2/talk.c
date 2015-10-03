#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <pthread.h>

#include <ncurses.h>

#define PORT 5555
#define MESSAGE_BUFFER 32

static volatile int keepRunning = 1;

struct ThreadVariables{
	int *sockfd;
};

void sig_chld(int sig){
	if(sig == SIGINT){
		keepRunning = 0;
	}
	signal(sig, sig_chld);
}

ssize_t writen(int fd, const void *vptr, size_t n)
{
	size_t nleft;
	ssize_t nwritten;
	const char *ptr;
	ptr = vptr;
	nleft = n;
	while (nleft > 0) {
		if ((nwritten = write(fd, ptr, nleft)) <= 0 ) {
			if (errno == EINTR)
				nwritten = 0; /* and call write() again */
			else
				return -1; /* error */
		}
		nleft -= nwritten;
		ptr += nwritten;
	}
	return n;
}

void display(char *str){
	char *tmp;
	for (tmp=str;*tmp;tmp++){
		
		write(1,tmp,1);
		usleep(100);
	}
}

void *readFrom(void *parm){
	struct ThreadVariables *args = (struct ThreadVariables *)parm;
	
	int err;
	char message[2];
	while(keepRunning){
		message[0] = 0;
		message[1] = 0;

		err = read(*args->sockfd, message, 2);
		if(err < 0){
			perror("Could not read");
			exit(1);
		}
		if(err == 0 || message[0] == 3){
			keepRunning = 0;
			break;
		}
		if(message[0] == 10){
			display("\r");
		}
		display(message);
	}
	return 0;
}

void *writeTo(void *parm){
	struct ThreadVariables *args = (struct ThreadVariables *)parm;
	int c = 0;
	char message[2];
	int err;
	signal(SIGINT, sig_chld);
	message[1] = 0;
	initscr();
	cbreak();
	while(keepRunning){
		if((c = getch()) == ERR){
			continue;
		}
		message[0] = (char) c;
		if(message[0] == 3){
			keepRunning = 0;
			//break;
		}
		else if(message[0] == 10 || message[0] == 13){
			message[0] = 10;
			display("\n\r");
		}
		
		err = writen(*args->sockfd, message, 2);
		if(err < 0){
			perror("Error writing message");
			exit(1);
		}
	}
	endwin();
	shutdown(*args->sockfd, SHUT_RDWR);
	return 0;
}

void server(int sockfd){
	int bind_err, listen_err, close_err, clientSockfd;

	struct sockaddr_in server_addr, client_addr;
	socklen_t server_addrlen, client_addrlen;
	
	pthread_t readThread, writeThread;
	pthread_attr_t attr;
	struct ThreadVariables threadVariables;

	
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	server_addrlen = (socklen_t) sizeof(struct sockaddr_in);

	bind_err = bind(sockfd, (struct sockaddr *) &server_addr, server_addrlen);
	if(bind_err < 0){
		perror("Could not bind to socket");
		exit(1);
	}
	
	listen_err = listen(sockfd, 5);
	if(listen_err < 0){
		perror("Could not listen to socket");
		exit(1);
	}
	
	client_addrlen = (socklen_t) sizeof(struct sockaddr_in);
	clientSockfd = accept(sockfd, (struct sockaddr *) &client_addr, &client_addrlen);
	if(clientSockfd < 0){
		perror("Could not accept from client");
		exit(1);
	}
	display("Client connected");
	threadVariables.sockfd = &clientSockfd;
	pthread_attr_init(&attr);
	pthread_create(&readThread, &attr, readFrom, (void *)&threadVariables);
	pthread_create(&writeThread, &attr, writeTo, (void *)&threadVariables);
	
	pthread_join(readThread, NULL);
	pthread_join(writeThread, NULL);
	
	close_err = close(sockfd);
	if(close_err < 0){
		perror("Could not close socket");
		exit(1);
	}
}

void client(int sockfd, char* loc){
	struct hostent *resolv;
	struct in_addr *addr;
	struct sockaddr_in server_addr;
	
	pthread_t readThread, writeThread;
	pthread_attr_t attr;
	struct ThreadVariables threadVariables;
	
	int conn_err, close_err;
	
	resolv = gethostbyname(loc);
	if(resolv == NULL){
		printf("Address not found for %s\n", loc);
		exit(1);
	}
	addr = (struct in_addr*) resolv->h_addr_list[0];
	
	server_addr.sin_family       = AF_INET;
	server_addr.sin_port         = htons(PORT);
	server_addr.sin_addr.s_addr  = inet_addr(inet_ntoa(*addr));
	conn_err = connect(sockfd, (struct sockaddr *) &server_addr, (socklen_t) sizeof(struct sockaddr_in));
	if(conn_err < 0){
		perror("Could not connect");
		exit(1);
	}
	
	display("Connected to server");
	threadVariables.sockfd = &sockfd;
	pthread_attr_init(&attr);
	pthread_create(&readThread, &attr, readFrom, (void *)&threadVariables);
	pthread_create(&writeThread, &attr, writeTo, (void *)&threadVariables);
	
	pthread_join(readThread, NULL);
	pthread_join(writeThread, NULL);
	
	close_err = close(sockfd);
	if(close_err < 0){
		perror("Could not close socket");
		exit(1);
	}
	
}

int main(int argc, char **argv){
  int sockfd;

  /*check for correct input*/
  if(argc>2){
    fprintf(stderr,"Usage: talk [server address]");
    exit(1);
  }
  
  /*create socket*/
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if(sockfd<0){
    perror("Error creating socket");
    exit(1);
  }
  
 
  /*decide whether process is client or server*/
  if(argc==1){ 
    server(sockfd);
  }else{
    client(sockfd, argv[1]);
  }
  return 0;
}
