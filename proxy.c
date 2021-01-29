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

char *node;
char *service;
struct addrinfo *hints;
struct addrinfo **res;
int serv_sock = 0, sock = 0, client_sock = 0;


int cleanup(){
	if(client_sock != 0){
		if(-1 == close(client_sock)){
			fprintf(stderr, "Error[23]:"
					" Failed to close proxy-client socket connection\n"
					"\t'%s'\n", strerror(errno));
			return -23;
		}
	}

	if(sock != 0){
		
		if(-1 == close(sock)){
			fprintf(stderr, "Error[24]:Failed to close proxy socket\n"
					"\t'%s'\n", strerror(errno));
			return -24;
		}
	}

	if(serv_sock != 0){
		if(-1 == close(serv_sock)){
			fprintf(stderr, "Error[25]:"
					" Failed to close proxy-server connection\n"
					"\t'%s'\n", strerror(errno));
			return -25;
		}
	}
	freeaddrinfo(*res);
	return 0;
}

int setup(){
	res = (struct addrinfo **)malloc(sizeof(struct addrinfo*));
	hints = (struct addrinfo *)malloc(sizeof(struct addrinfo));
	memset(hints, 0, sizeof(*hints));
	hints->ai_family = AF_UNSPEC;
	hints->ai_socktype = SOCK_STREAM;
	hints->ai_flags = AI_PASSIVE;


	int result = getaddrinfo(node, service, hints, res);
	
	if(0 != result){
		fprintf(stderr,"Error[26]: failed to get address for %s:%s\n"
				"\t%s\n", node, service, strerror(result));
		cleanup();
		return -26;

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
		fprintf(stderr, "Error[27]: could not create host socket\n"
				"\t'%s'\n", strerror(errno));
		cleanup();
		return -27;
	}

	char hostbuffer[256], *IPbuffer;
	struct hostent * host_entry;

	gethostname(hostbuffer, sizeof(hostbuffer));
	host_entry = gethostbyname(hostbuffer);
	IPbuffer = inet_ntoa(*((struct in_addr*) host_entry->h_addr_list[0]));

	printf("Proxy IP: %s\nProxy Port: %s\n", 
			IPbuffer, service);

	int serv_val = atoi(service);
	sprintf(service, "%d", serv_val + 1);
	result = getaddrinfo(NULL, service, hints, res);
	
	if(0 != result){
		fprintf(stderr,"Error[28]: failed to get address for %s:%s\n"
				"\t%s\n", node, service, strerror(errno));
		cleanup();
		return -28;

	}

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
		fprintf(stderr, "Error[29]: could not create host socket\n"
				"\t'%s'\n", strerror(errno));
		cleanup();
		return -29;
	}
	int optval = 1;
	if( -1 == setsockopt(
			sock,
		   	SOL_SOCKET,
			SO_REUSEADDR,
			&optval,
			sizeof(optval))){
		fprintf(stderr, "Error[30]:Failed to make port reusable for proxy\n"
				"\t'%s'\n", strerror(errno));
		cleanup();
		return -30;
	}
	// bind socket to self
	if(-1 == bind(sock, (*res)->ai_addr, (*res)->ai_addrlen)){
		fprintf(stderr, "Error[31]: could not bind host socket\n"
				"\t'%s'\n", strerror(errno));
		cleanup();
		return -31;
	}

	return 0;
}

int lookup(char * req){
	if(
			0 == (strncmp(req, "mon", 10) || strncmp(req, "monday", 10))
			|| 0 == (strncmp(req, "tues", 10) || strncmp(req, "tuesday", 10))
			|| 0 == (strncmp(req, "wed", 10) || strncmp(req, "wednesday", 10))
			|| 0 == (strncmp(req, "thurs", 10) || strncmp(req, "thursday", 10))
			|| 0 == (strncmp(req, "fri", 10) || strncmp(req, "friday", 10))
			|| 0 == (strncmp(req, "sat", 10) || strncmp(req, "saturday", 10))
			|| 0 == (strncmp(req, "sun", 10) || strncmp(req, "sunday", 10))
	  ){
		return 1;
	}
	else if(
			0 == (strncmp(req, "a", 10) || strncmp(req, "all", 10))
		   ){
		return 2;
	}
	else if(
			0 == (strncmp(req, "q", 10) || strncmp(req, "quit", 10))
		   ){
		return 3;
	}
	else{
		return -1;
	}
}

int serve(){
	int should_close = 1;
	struct sockaddr_storage response_addr;

	while(1 == should_close){
		// await incoming connections
		if(-1 == listen(sock, 10)){
			fprintf(stderr, "Error[32]: Failed to listen for incoming connections\n"
					"\t'%s'\n", strerror(errno));
			cleanup();
			return -32;
		}
		// Accpet a new connection
		socklen_t addrlen = sizeof(response_addr);
		client_sock = accept(sock, (struct sockaddr *)&response_addr, 
					&addrlen);
		if(-1 == client_sock){
			fprintf(stderr, "Error[33]: Failed to accept incoming connection\n"
					"\t'%s'\n", strerror(errno));
			cleanup();
			return -33;
		}
		// create a buffer for incoming requests
		char  request[10];
		memset(request, '\0', 10);
		int result = recv_w_err(client_sock, request, 10);
		if(-1 != result){
			// check the request for 1 of 4 conditions
			// 3: q/quit
			// 2: a/all
			// 1: single day
			// -1: erroneous request
			result = lookup(request);
			int len;
			
			// exit condition
			if( 3 == result ){
				printf("Server exiting...\n");
				len = strlen(request);

				// pass on the exit message to true server, and cleanup
				result = send_w_err(serv_sock, request, len);
				if( -1 == result ){
					cleanup();
					return -36;
				}
				cleanup();
				return 0;
				
			}

			// all condition
			else if( 2 == result ){
				char *days[7] = {
					"mon", "tues", "wed", "thurs", "fri", "sat", "sun"
				};
				char * response = (char *)(malloc(sizeof(char)*
							strlen("\tTemperature(deg. C)\tPrecipitation\n\0")
							+ 6*7 // temp/precip values 
							// ^ len = 4, +2/day for tabs, +1/day for null
							+ 4*5 // 5 days with length 3 abbv. +1/day for null
							+ 5*2 // 2 days with length 4 abbv. +1/day for null)
							+ 7 // add additional space for newlines
						));

				memcpy(response, "\tTemperature(deg. C)\tPrecipitation\n\0",
						strlen("\tTemperature(deg. C)\tPrecipitation\n\0"));

				// iterate through days and obtain weather
				for(int i = 0; i < 7; i += 1){

					len = strlen(days[i]);
					char  temp[5] = {"\0"};


					// request for ith day of week weather
					result = send_w_err(serv_sock, days[i], len);
					if( -1 == result ){
						fprintf(
								stderr, 
								"Error[36.%d]:Failed to send request for '%s'\n"
								"\t'%s'\n", i, days[i], strerror(errno));
					}

					// obtain and buffer result
					result = recv_w_err(serv_sock, temp, 4);
					if(-1 == result){
						fprintf(stderr,
								"Error[37.%d]: Failed to receive server response"
								" on proxy side for request '%s'\n"
								"\t'%s'", i, days[i], strerror(errno));
					}

					else{
						strncat(response, "\n", 2); 
						strncat(response, days[i], strlen(days[i]));
						strncat(response, "\t", 2);
						strncat(response, temp, 2);
						strncat(response, "\t", 2);
						strncat(response, &(temp[2]), 2);

						//above results in response at this iteration 'idx'having
						// '\n$day\t$temp\t$precip' added to the buffer
					}
				}

				// send response to client
				if(-1 == send_w_err(client_sock, response, strlen(response))){
					fprintf(stderr, 
							"Error[38]: Failed to send all response to client\n"
							"\t'%s'\n", strerror(errno));
				}
				free(response);
				response = NULL;
			}

			// passthrough condition
			else if(1 == result){
				char * response = (char *)malloc(sizeof(char) * 5);
				result = send_w_err(
						serv_sock, 
						request,
						strlen(request)
						);
				if(-1 == result){

					fprintf(stderr, 
							"Error[38]: Failed to pass request on to server\n"
							"\t'%s'\n", strerror(errno));
				
				}
				else{

					result = recv_w_err(serv_sock, response, 4);

					if(-1 == result){
						fprintf(stderr,
								"Error[39]: Failed to receive passthrough"
								" response from server\n\t'%s'\n", 
								strerror(errno));
					}
					else{
						result = send_w_err(
								client_sock, response, strlen(response));

						if(-1 == result){
							fprintf(stderr,
									"Error[40]: Failed to send passthrough"
									" response to client\n\t'%s'\n",
									strerror(errno));
						}

					}
				}
				free(response);
				response = NULL;

			}	
			// erroneous condition
			else if ( -1 == result ){
				fprintf(stderr, "Server exiting on error...\n");
				cleanup();
				return result;
			}

		}

	}

	return 0;
}

int main(int argc, char * argv[]){

	if(3 != argc){
		fprintf(stderr, "Error[-2]: Invalid number of arguments provided to Proxy\n"
				"\tPlease input Server IP and Port in that order\n");
		return 2;
	}

	node = (char*)malloc(sizeof(char)*strlen(argv[1]));
	service = (char*)malloc(sizeof(char)*strlen(argv[2]));

	strncpy(node, argv[1], strlen(argv[1]));
	strncpy(service, argv[2], strlen(argv[2]));

	int result = setup();
	if(0 != result){
		return result;
	}

	return serve();
}
