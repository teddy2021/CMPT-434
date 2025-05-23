// CMPT 434
// ASSIGNMENT 1
// KODY MANASTYRSKI
// KOM607
// 11223681
//

#include <string.h>
#include <sys/socket.h>

#ifndef COM_H
#define COM_H
#endif

extern const int offset, response_size;
int send_w_err(int sock, char * message, int len);
int recv_w_err(int sock, char * buffer, int len);
int sendto_w_err(int sock, char* message, int len, struct sockaddr *to, socklen_t to_len);
int recvfrom_w_err(int sock, char * buffer, int len, struct sockaddr *from, socklen_t *from_len);
