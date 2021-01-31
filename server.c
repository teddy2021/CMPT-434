// CMPT 434
// ASSIGNMENT 1
// KODY MANASTYRSKI
// KOM607
// 11223681
//

// Input Reading
#include <stdio.h>
#include <string.h>
#include <ctype.h>

// Memory
#include <stdlib.h>
#include <unistd.h>

// Networking
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#ifndef COM_H
#define COM_H
#include "common.h"
#endif

char *node = "0.0.0.0";
char *service;
struct addrinfo *hints;
struct addrinfo **res;
int sock = 0;
int client_sock = 0;
int tcp;
extern const int response_size;

int cleanup(){

	printf("Server exiting...\n");
	if(sock != 0){
		
		if(-1 == close(sock)){
			fprintf(stderr, "Error[22]:Failed to close server socket\n"
					"\t'%s'\n", strerror(errno));
			return -22;
		}
	}	

	if(client_sock != 0){
		if(-1 == close(client_sock)){
			fprintf(stderr, "Error[21]:"
					" Failed to close server-client socket connection\n"
					"\t'%s'", strerror(errno));
			return -21;
		}
	}
	fflush(stdout);
	fflush(stderr);
	freeaddrinfo(*res);
	return 0;
}

int setup(){
	res = (struct addrinfo **)malloc(sizeof(struct addrinfo*));
	hints = (struct addrinfo *)malloc(sizeof(struct addrinfo));
	memset(hints, 0, sizeof(*hints));
	hints->ai_family = AF_UNSPEC;
	if(0 == tcp){
		hints->ai_socktype = SOCK_STREAM;
	}
	else{
		hints->ai_socktype = SOCK_DGRAM;
	}
	hints->ai_flags = AI_PASSIVE;



	int result = getaddrinfo(node, service, hints, res);
	
	if(0 != result){
		fprintf(stderr,"Error[10]: failed to get address for %s:%s\n"
				"\t%s\n", node, service, strerror(result));
		cleanup();
		return -10;

	}

	struct addrinfo * p;
	char ipstr[INET6_ADDRSTRLEN];
	for(p = *res; p != NULL; p = p->ai_next){
		void *addr;

		if(p->ai_family == AF_INET){
			struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
			addr = &(ipv4->sin_addr);
		}
		else{
			struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
			addr = &(ipv6->sin6_addr);
		}

		inet_ntop(p->ai_family, addr, ipstr, sizeof(ipstr));

	}

	// obtain a socket to self
	sock = socket((*res)->ai_family, (*res)->ai_socktype, (*res)->ai_protocol);
	if(-1 == sock){
		fprintf(stderr, "Error[11]: could not create host socket\n"
				"\t'%s'\n", strerror(errno));
		cleanup();
		return -11;
	}

	int optval = 1;
	if(-1 == setsockopt(
				sock, 
				SOL_SOCKET, 
				SO_REUSEADDR, 
				&optval, 
				sizeof(optval))){
		fprintf(stderr, "Error[12]: could not make the port reusable\n"
				"\t'%s'\n", strerror(errno));
		cleanup();
		return -12;
	}


	// bind socket to self
	if(-1 == bind(sock, (*res)->ai_addr, (*res)->ai_addrlen)){
		fprintf(stderr, "Error[13]: could not bind host socket\n"
				"\t'%s'\n", strerror(errno));
		cleanup();
		return -13;
	}

	char hostbuffer[256], *IPbuffer;
	struct hostent * host_entry;

	gethostname(hostbuffer, sizeof(hostbuffer));
	host_entry = gethostbyname(hostbuffer);
	IPbuffer = inet_ntoa(*((struct in_addr*) host_entry->h_addr_list[0]));


	printf("Server IP: %s\nServer Port: %s\n", 
			IPbuffer, service);
	fflush(stdout);

	if(-1 == listen(sock, 10)){
		fprintf(stderr, "Error[15]: Failed to listen for incoming connections\n"
				"\t'%s'\n", strerror(errno));
		return -14;
	}

	struct sockaddr_storage response_addr;
	socklen_t addrlen = sizeof(response_addr);
	
	client_sock = accept(sock, (struct sockaddr *)&response_addr, 
				&addrlen);
	if(-1 == client_sock){
		fprintf(stderr, "Error[16]: Failed to accept incoming connection\n"
				"\t'%s'\n", strerror(errno));
		cleanup();
		return -15;
	}

	return 0;
}

int lookup(char * request, char ** response){
	char * outstr;
	if( 0 == strncmp(request, "mon", 10) || 
			0 == strncmp(request, "monday", 10)){
		outstr = "\t\t12\t\t34\0";
	}
	else if( 0 == strncmp(request, "tues", 10) ||
			0 == strncmp(request, "tuesday", 10) ){
		outstr = "\t\t56\t\t78\0";
	}
	else if( 0 == strncmp(request, "wed", 10) || 
			0 == strncmp(request, "wednesday", 10) ){
		outstr = "\t\t91\t\t01\0";
	}
	else if( 0 == strncmp(request, "thurs", 10) ||
			0 == strncmp(request, "thursday", 10) ){
		outstr = "\t\t11\t\t21\0";
	}
	else if ( 0 == strncmp(request, "fri", 10) ||
			0 == strncmp(request, "friday", 10)	){
		outstr = "\t\t31\t\t41\0";
	}
	else if ( 0 == strncmp(request, "sat", 10) ||
			0 == strncmp(request, "saturday", 10) ){
		outstr = "\t\t51\t\t61\0";
	}
	else if ( 0 == strncmp(request, "sun", 10) ||
			0 == strncmp(request, "sunday", 10)	){
		outstr = "\t\t71\t\t81\0";
	}
	else if ( 0 == strncmp(request, "q", 10) || 
			0 == strncmp(request, "quit", 10)){
		return 1;
	}
	else{
		memset(*response, '\0', strlen(*response));
		strncpy(*response, 
				"invalid server request\n", 
				strlen("invalid server request\n")+1);

		fprintf(stderr, "Error[14]: invalid request ('%s')\n", request);
		return -14;
	}
	strncat(*response, outstr, strlen(outstr));
	return 0;
}

int serve(){
	int should_close = 1;

	while(1 == should_close){

		char  request[10];
		memset(request, '\0', 10);
		int result = 0;
		
		if(0 == tcp){
			result = recv_w_err(client_sock, (void*)request, 10);
		}
		else{
			result = recvfrom_w_err(client_sock, (void*)request, 10);
		}

		printf("Server received %s request\n", request);
		
		if(result == 0){
			char * response = malloc(sizeof(char) * response_size); 
	
			strncpy(response, "Temperature(deg. C)\tPrecipitation\n",
					strlen("Temperature(deg. C)\tPrecipitation\n") + 1);
			
			result = lookup(request, &response);


			if( 1 == result ){
				printf("Server exiting...\n");
				return cleanup();
			}

			int len = strlen(response);

			if(tcp == 0){
				send_w_err(client_sock, response, len);
			}
			else{
				sendto_w_err(client_sock, 
						response, len,
						NULL, 0);
			}
			memset(response, '\0', response_size);
		}
	}
	return 0;
}

int main(int argc, char *argv[]){
	if(argc != 3){
		fprintf(stderr, "Error[-1]: Invalid amount of arguments to Server\n"
				"\tExpected 2 arguments, got %d\n"
				"\tPlease input port and protocol (-T or -U)\n", (argc -1));
		return 1;
	}


	service = (char*)malloc(sizeof(char) * strlen(argv[1]));
	strncpy(service, argv[1], strlen(argv[1]));
	
	char* protocol_flag = (char*)malloc(sizeof(char)*3);
	strncpy(protocol_flag, argv[2], strlen(argv[2]));

	if(0 == strncmp(protocol_flag, "-T", 2)){
		tcp = 0;
	}
	else if( 0 == strncmp(protocol_flag, "-U", 2) ){
		tcp = 1;
	}
	else{
		fprintf(stderr, "Incorrect protocol flag provided\n\tFlag: %s\n"
				"\tValid Flags: '-T', '-U'\n", protocol_flag);
		return 5;
	}

	int result = setup();
	if(0 != result){
		return result;
	}

	return serve();
}
