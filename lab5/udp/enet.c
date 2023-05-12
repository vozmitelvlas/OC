#include "enet.h"

int mn_recv(int sock, char *client_message, int *mn, struct mes_buf mesbuf[100], struct timeval *timeout, fd_set *readset){
    char msg_with_n[BUFLEN], tmp[BUFLEN];
    int read_size, new_mn, buf_read_size, reason;

    while(reason = select(sock+1, readset, NULL, NULL, timeout) > 0){

    if( (read_size = recv(sock , msg_with_n, BUFLEN , 0)) > 0){
        if(strncmp(msg_with_n,"BEY",3) == 0){
            return -1;           
        }
        new_mn = get_mn(msg_with_n);
        if( (buf_read_size = get_from_buf(*mn+1, mesbuf, tmp)) > 0){
            *mn = *mn + 1;
            strcpy(client_message, tmp);
            return buf_read_size;
        }
        else{
            strcpy(tmp, msg_with_n+4);
            if( new_mn == *mn + 1){
                *mn = *mn + 1;
                strcpy(client_message, tmp);
                return strlen(client_message);
            }
            else{
                if(new_mn > *mn + 1){
                    add_to_buf(new_mn, tmp, mesbuf);
                }
                if(new_mn < *mn + 1) puts("duplicated message");
            }
        }
    }

    }
    
    if( (buf_read_size = get_from_buf(*mn+1, mesbuf, tmp)) > 0){
        *mn = *mn + 1;
        return buf_read_size;
    }
    
    return reason;

}

int get_mn(char msg[BUFLEN]){
    return strtoul(msg, NULL,10);
}

int add_to_buf(int mn, char *msg, struct mes_buf mesbuf[100]){
  for(int i = 0; i < 100; i++){
    if(mesbuf[i].number == mn){
        puts("buffer: duplicated message");
        return -1;
    }
  }
  for(int i = 0; i < 100; i++){
    if(mesbuf[i].number == 0){
        mesbuf[i].number = mn;
        strncpy(mesbuf[i].msg, msg, BUFLEN);
        puts("outrunnig message buffered");
        return 0;
    }
  }
  puts("No free space in buffer");
  return -2;
}

int get_from_buf(int nm, struct mes_buf mesbuf[100], char *tmp){
    char n[3];
    for(int i = 0; i < 100; i++){
        if(mesbuf[i].number == nm){
            strcpy(tmp, mesbuf[i].msg+4);
            bzero(mesbuf[i].msg, BUFLEN);
            puts("buffer: here i come");
            return strlen(tmp);
        }
    }
    return 0;
}

// Receiving part

int nprint(int n, char *message){
  if (n < 0) return -1;
  strncpy(message, "000 ", 4);
  if(n < 10) sprintf(message+2, "%d", n);
  else if(n < 100) sprintf(message+1, "%d", n);
  else if(n < 1000) sprintf(message, "%d", n);
  else return -2;
  return 0;
}

int return_bigger(int recv_size, int buf_size){
    if(recv_size > buf_size) return recv_size;
    else return buf_size;
}

int nsend(int *mn, int sock , char *message){
    char n[BUFLEN];
    strncpy(n, "000", 3);
    nprint(inc_mn(mn), n);
    n[3] = ' ';
    strncpy(n+4, message, BUFLEN-4);
    strncpy(message, n, BUFLEN);
    if( send(sock , message, BUFLEN , 0) < 0){
        puts("Send failed");
        return 1;
    }
    return 0;
}

int inc_mn(int *mn){
  if(*mn<1000) *mn = *mn + 1;
  else *mn = 0;
return *mn;
}
