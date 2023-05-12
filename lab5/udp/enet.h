#ifndef ENET_H_
#define ENET_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/time.h>

#define BUFLEN 512

struct mes_buf{
    int number;
    char msg[BUFLEN];
};

int mn_recv(int , char *, int *, struct mes_buf [], struct timeval *, fd_set *);
int get_mn(char msg[BUFLEN]);
int add_to_buf(int, char *, struct mes_buf mesbuf[]);
int nprint(int, char *);
int get_from_buf(int , struct mes_buf mesbuf[], char *);
int return_bigger(int, int);

int nsend(int *, int , char*);
int nprint(int , char *);
int inc_mn(int *);

#endif // ENET_H_
