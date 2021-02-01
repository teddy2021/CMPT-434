// CMPT 434
// ASSIGNMENT 1
// KODY MANASTYRSKI
// KOM607
// 11223681
//

#ifndef COM_H
#define COM_H
#include "common.h"
#endif

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <sys/socket.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netdb.h>


const int offset = strlen("Temperature(deg. C)\tPrecipitation\n");
const int response_size = offset 
	+ 7 * 4 // 14 newlines and 14 tabs
	+ 5 * 4 // 5 three letter abbvs + 1 newline character each
	+ 2 * 5 // 2 four letter abbvs + 1 newline character each
	+ 7 * 4; // 7 four character sequences of responses


int send_w_err(int sock, char * message, int len){
	int bytes_sent = 0, cumulat_sent = 0, remaining = len;

	while(remaining != 0){
		bytes_sent = send(
				sock,
				(void*)message + cumulat_sent,
				len - cumulat_sent,
				0
				);
		if(-1 == bytes_sent){
			fprintf(stderr,
					"Error[7]: Failed to send message\n"
					"\t'%s'\n", strerror(errno));
			return -7;
		}
		cumulat_sent += bytes_sent;
		remaining = len - cumulat_sent;
	}
	return 0;
}


int recv_w_err(int sock, char * buffer, int len){
	if(-1 == recv(
				sock, 
				(void*)buffer,
				len,
				0
				)){
		fprintf(stderr,
				"Error[8]: Failed to receive message"
				"\t'%s'\n", strerror(errno));
		return -8;
	}
	return 0;
}


int sendto_w_err(int sock, char* message, int len, 
		struct sockaddr *to, socklen_t to_len){
	int bytes_sent = 0, cumulat_sent = 0, remaining = len;

	while(remaining != 0){
		bytes_sent = sendto(
				sock,
				(void*)message + cumulat_sent,
				len - cumulat_sent,
				0,
				to,
				to_len
				);
		if(-1 == bytes_sent){
			fprintf(stderr,
					"Error[7]: Failed to send message\n"
					"\t'%s'\n", strerror(errno));
			return -7;
		}
		cumulat_sent += bytes_sent;
		remaining = len - cumulat_sent;
	}
	return 0;
}


int recvfrom_w_err(int sock, char * buffer, int len, 
		struct sockaddr *from, socklen_t * from_len){
	char dst[INET_ADDRSTRLEN];

	if(-1 == recvfrom(sock,(void*)buffer, len,
				0, from, from_len)){

		inet_ntop(from->sa_family, ((struct in_addr *)from), dst, 
				INET_ADDRSTRLEN);
		fprintf(stderr,
				"Error[8]: Failed to receive message from %s\n"
				"\t'%s'\n", dst, strerror(errno));
		return -8;
	}
	return 0;
}

