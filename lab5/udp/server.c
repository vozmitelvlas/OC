#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<pthread.h>
#include "enet.h"

#define SCK "\x1B[34m"
#define TRD "\x1B[33m"
#define ERR "\x1B[31m"
#define RST "\033[0m"

struct client_descriptor
{
	int	socket;
	pthread_t thread;
    struct sockaddr_in client;
};
struct client_descriptor client_d[100];
pthread_mutex_t lock;

void *connection_handler(void *);
void *accept_handler(void *);
int add_sockaddr(int, struct sockaddr_in);
int add_tdescriptor(int , pthread_t);
int shut(int id);
void list();
int free_client_desc();
int sub_connect(int, int);
int add_sockdesc(int, int);

int main(int argc , char *argv[])
{
    int socket_desc , status_addr;
    struct sockaddr_in server;
    char message[1000];

    //Create socket
    socket_desc = socket(AF_INET , SOCK_DGRAM, IPPROTO_UDP);
    if (socket_desc == -1){
        puts(ERR"Main socket: could not create"RST);
    }
    puts(SCK"Main socket: created"RST);

    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 8888 );

    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0){
        //print the error message
        puts(ERR"Main socket: could not bind"RST);
        return 1;
    }
    puts(SCK"Main socket: binded"RST);

    //Accept and incoming connection
    puts(SCK"Main socket: waiting"RST);

    pthread_t accept_thread;

    if( pthread_create( &accept_thread , NULL , accept_handler , (void*) socket_desc) < 0)
        {
            puts(ERR"Main thread: failed to create new thread"RST);
            return 1;
        }

    while(1){
        bzero(message, sizeof message);
        fgets (message, sizeof message, stdin);
	    if(strncmp(message,"quit",4) == 0){ break;}
        else if(strncmp(message,"shut ",5) == 0){
            shut(strtoul(message+5, NULL,10));
        }
        else if(strncmp(message,"list",4) == 0){
            list();
        }
        else printf(ERR"Wrong command"RST"\n");
    }


    if (connect(socket_desc , (struct sockaddr *)&server , sizeof(server)) < 0)
        puts("failed to connect main socket to itself");
    if(shutdown(socket_desc, SHUT_RDWR) == 0){
	    puts(SCK"Main socket: down"RST);
	    if(close(socket_desc) == 0){
	        puts(SCK"Main socket: closed"RST);
	    }
	    else{
	        puts(ERR"Main socket: failed to close "RST);
	        return 1;
	    }
    }
    else{
        puts(ERR"Main socket: failed to shut down"RST);
        return 1;
    }

    if (pthread_join(accept_thread , (void**)&status_addr) != 0){
        puts("Main thread: can't join accept thread");
    }

    puts("Main thread termination phase");
    return 0;
}

void *connection_handler(void *the_id){
    //Get the socket descriptor
    int id = *(int*)the_id;
    int sock, reason, time_to_wait = 20, mn = 0, rmn = 0;
    char client_message[BUFLEN];
    char reply[BUFLEN];
    struct timeval timeout;
    fd_set readset;
    struct mes_buf mesbuf[100];
    //struct sockaddr_in me;
    //socklen_t melen = sizeof(me);

    timeout.tv_sec = time_to_wait;

    if ((sock=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1)
    puts(ERR"Subsocket: failed to create"RST);
    //getsockname(sock, &me, &melen);
    //printf(TRD"Subthread[%d]: new port is %d"RST"\n", id, ntohs(me.sin_port));
    add_sockdesc(id, sock);
    if(sub_connect(id, sock) == -1)
    puts(ERR"Subsocket: failed to connect"RST);
    bzero(client_message, sizeof client_message);
    FD_ZERO(&readset);
    FD_SET(sock, &readset);
    //Receive a message from client
    while( ((reason = mn_recv(sock ,client_message, &mn, &mesbuf, &timeout, &readset)) > 0) ){
	nsend(&rmn, sock , client_message);
	bzero(client_message, sizeof client_message);
        bzero(reply, sizeof reply);
        timeout.tv_sec = time_to_wait;
    }
    switch(reason){
    case 0 :
        printf(TRD"Subthread[%d]: client was inactive for too long"RST"\n", id);
        break;
    case -1 :
        printf(TRD"Subthread[%d]: client ended session"RST"\n", id);
        break;
    }

    pthread_mutex_lock(&lock);
    client_d[id].socket = 0;
    pthread_mutex_unlock(&lock);
    printf(TRD"Subthread[%d]: client disconnected"RST"\n", id);
}

void *accept_handler(void *socket_desc){
    int c , id, *new_id;
    char buf[BUFLEN];
    struct sockaddr_in new_client;
    c = sizeof(new_client);
AH_RPT:
    while( recvfrom(socket_desc, buf, BUFLEN, 0, &new_client, &c) > 0){
        puts(SCK"Main socket: connection accepted"RST);

        id = free_client_desc();
        if (id < 0){
            puts(ERR"Main socket: all subsockets are busy"RST);
        }
        else{
            if (add_sockaddr(id ,new_client) < 0){
                puts(ERR"Main socket: all subsockets are busy"RST);
            }
            else{
                pthread_t sniffer_thread;
                new_id = malloc(1);
                *new_id = id;

                if( pthread_create( &sniffer_thread , NULL , connection_handler , (void*) new_id) < 0){
                    puts(ERR"Main thread: failed to create new thread"RST);
                    goto AH_RPT;
                }
                add_tdescriptor(id, sniffer_thread);

                printf(TRD"Main thread: new thread created, id = %d, ip = %s, port = %d"RST"\n", id, inet_ntoa(new_client.sin_addr) , ntohs(new_client.sin_port));
            }
        }
    }
AH_END:
printf("recvfrom return value: %d\n", recvfrom(socket_desc, buf, BUFLEN, 0, &new_client, &c));
    for(int i=0; i < 100; i++){
        if(client_d[i].socket != 0){
	        shut(i);
        }
    }

    for(int i=0; i < 100; i++){
        if(client_d[i].socket != 0){
	        pthread_join( client_d[i].thread , NULL);
        }
    }
}

int free_client_desc(){
    int id = -1;
    pthread_mutex_lock(&lock);
    for(int i=0; i < 100; i++){
        if(client_d[i].socket == 0){
            id = i;
            break;
        }
    }
    pthread_mutex_unlock(&lock);
    return id;
}

int add_sockaddr(int id, struct sockaddr_in client){
    pthread_mutex_lock(&lock);
    client_d[id].client = client;
    pthread_mutex_unlock(&lock);
    return 0;
}

int add_sockdesc(int id, int desc){
    pthread_mutex_lock(&lock);
    client_d[id].socket = desc;
    pthread_mutex_unlock(&lock);
    return 0;
}

int add_tdescriptor(int id, pthread_t sniffer_thread){
    if(id < 0){return 1;}
    pthread_mutex_lock(&lock);
    client_d[id].thread = sniffer_thread;
    pthread_mutex_unlock(&lock);
    return 0;
}

int sub_connect(int id, int socket){
    pthread_mutex_lock(&lock);
    int size = sizeof(client_d[id].client);
    if (sendto(socket, "ACK", BUFLEN, 0, &client_d[id].client, size) < 0)
        return -2;
    if (connect(socket , (struct sockaddr *)&client_d[id].client , size) < 0){
        perror("connect failed. Error");
        pthread_mutex_unlock(&lock);
        return -1;
    }
    pthread_mutex_unlock(&lock);
    return 0;
}


int shut(int id){
    pthread_mutex_lock(&lock);
    if(client_d[id].socket != 0){
        write(client_d[id].socket , "BEY\n\0" , BUFLEN);
        if(shutdown(client_d[id].socket, SHUT_RDWR) == 0){
        printf(SCK"Subsocket[%d]: down"RST"\n", id);
    	    if(close(client_d[id].socket) == 0){
            printf(SCK"Subsocket[%d]: closed"RST"\n", id);
    	    }
    	    else{
    		    printf(ERR"Subsocket[%d]: failed to close"RST"\n", id);
    		    goto SH_END;
    	    }
        }
        else{
	        printf(ERR"Subsocket[%d]: failed to shut down"RST"\n", id);
   	        goto SH_END;
        }
    }
SH_END:
    pthread_mutex_unlock(&lock);
    return 0;
}

void list(){
    pthread_mutex_lock(&lock);
    for(int i=0; i < 100; i++){
        if(client_d[i].socket != 0){
            printf(TRD"ID = %d, port = %d"RST"\n", i , ntohs(client_d[i].client.sin_port));
        }
    }
    pthread_mutex_unlock(&lock);
}

