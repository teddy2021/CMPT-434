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

const int offset = strlen("Temperature(deg. C)\tPrecipitation\n");
const int response_size = 
	offset 
	+ 7 * 3 // 7 newlines and 14 tabs
	+ 5 * 4 // 5 three letter abbvs + 1 newline character each
	+ 2 * 5 // 2 four letter abbvs + 1 newline character each
	+ 7 * 4; // 7 four character sequences of responses

int send_w_err(int sock, char * message, int len);
int recv_w_err(int sock, char * buffer, int len);
int sendto_w_err(int sock, char* message, int len, struct sockaddr *to, socklen_t to_len);
int recvfrom_w_err(int sock, char * buffer, int len);
