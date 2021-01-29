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

#ifndef COM_H
#define COM_H
#include "common.h"
#endif

char *node;
char *service;
struct addrinfo *hints;
struct addrinfo **res;
int response_size = 
	strlen("Temperature(deg. C)\tPrecipitation\n")
	+ 7 * 3 // 7 newlines and 14 tabs
	+ 5 * 4 // 5 three letter abbvs + 1 null character each
	+ 2 * 5 // 2 four letter abbvs + 1 null character each
	+ 7 * 4; // 7 four character sequences of responses
int tcp;
int sock;


void setup(){
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

}

int connect_and_request(char * input, char ** response_buffer){
	int result = getaddrinfo(node, service, hints, res);
	
	if(0 != result){
		fprintf(stderr,"Error[4]: failed to get address for %s:%s\n"
				"\t%s\n", node, service, gai_strerror(result));
		return -4;

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

	// obtain a socket to our server
	sock = socket((*res)->ai_family, (*res)->ai_socktype, (*res)->ai_protocol);
	if(-1 == sock){
		fprintf(stderr, "Error[5]: could not create socket\n"
				"\t'%s'\n", strerror(errno));
		return -5;
	}

	result = connect(sock, (*res)->ai_addr, (*res)->ai_addrlen);
	if(-1 == result){
		fprintf(stderr, "Error[6]: could not connect to server\n"
				"\t'%s'\n", strerror(errno));
		return -6;
	}

	if(0 == tcp){
		result = send_w_err(sock, (void*)input, strlen(input));
	}
	else{
		result = sendto_w_err(sock, (void*)input, strlen(input),
				(*res)->ai_addr, (*res)->ai_addrlen);
	}

	if(0 == tcp){
		result = recv_w_err(sock, (void*)*response_buffer, response_size);
	}
	else{
		result = recvfrom_w_err(sock,(void*)response_buffer, response_size);
	}
	
	if(-1 == close(sock)){
		fprintf(stderr, "Error[9]: Failed to close connection\n"
				"\t'%s'\n", strerror(errno));
		return -9;
	}
	freeaddrinfo(*res);
	return 0;
}

int main(int argc, char* argv[]){
	// one time setup of hints
	if(argc < 4){
		fprintf(stderr,
			   	"Error[-3]: Invalid number of arguments provided to Client\n"
				"\tPlease input target IP, Port, Protocol('-T' or '-U') in that"
				" order\n");
		return 3;
	}
	
	node = (char *)malloc(sizeof(char)*strlen(argv[1]) + 1);
	service = (char *)malloc(sizeof(char)*strlen(argv[2]) + 1);
	char *flag = (char*)malloc(sizeof(char)*strlen(argv[3]) + 1);

	strncpy(node, argv[1], strlen(argv[1]));
	strncpy(service, argv[2], strlen(argv[2]));
	strncpy(flag, argv[3], strlen(argv[3]));

	if( 0 == strncmp(flag, "-T", 2) ){
		tcp = 0;
	}
	else if(0 == strncmp(flag, "-U", 2)){
		tcp = 1;
	}

	setup();


	int proceed = 1;
	char input[10];
	int result;
	// enter the REPL
	while (proceed){
		memset(input, '\0', 10);
		// Print input request
		printf("Enter a day of the week\nFormat should be (mon/tues/wed/thurs/"
			"fri/monday/tuesday/wednesday/thursday/friday)\n");
		// read input and error check input for length
		fgets(input, 100, stdin);
		int len = strlen(input);
		len -= 1;
		input[len] = '\0';
		
		if(0 == len || 10 < len){
			fprintf(stderr, "Error[1]: invalid input\n"
					"\tInput received was '%s'\n", input);
			return -1;
		}

		else{	
			input[9] = '\0';
			// Standardize input to lower case to simplify comparision cases
			for(int i = 0; i < 9; i += 1){
				if(input[i] != ' ' || input[i] != '\n'){
					input[i] = tolower(input[i]);
				}
				else if(input[i] == '\n'){
					input[i] = '\0';
				}
				else{
					fprintf(stderr, "Error[2]: unexpected character in input.\n"
							"\t'%c' is not acceptable input\n", input[i]);
					return -2;
				}
			}
			// check input for validity
			if(
					0 == strncmp(input,	"mon",			10)
				   || 0 == strncmp(input,	"monday",		10)
				   || 0 == strncmp(input,	"tues",			10)
				   || 0 == strncmp(input,	"tuesday",		10)
				   || 0 == strncmp(input,	"wed",			10) 
				   || 0 == strncmp(input,	"wednesday",		10)
				   || 0 == strncmp(input,	"thurs",			10) 
				   || 0 == strncmp(input,	"thursday",			10)
				   || 0 == strncmp(input,	"fri",			10) 
				   || 0 == strncmp(input,	"friday",			10)
				   || 0 == strncmp(input,	"sat",			10)
				   || 0 == strncmp(input,	"saturday",			10)
				   || 0 == strncmp(input,	"sun",			10)
				   || 0 == strncmp(input,	"sunday",			10)
				   ){
				char * response = (char*)malloc(sizeof(char) * 20);
				
				result = connect_and_request(input, &response);
				if(result != 0){
					return result;
				}


				printf("%s\n", response);

				free(response);
				response = NULL;

				memset(&input, '\0', sizeof(input));

			}
			else if(0 == strncmp(input, "q", 12) || 
					0 == strncmp(input, "quit", 12)){
				printf("Exiting...\n");
				result = send_w_err(sock, "q", 1);
				if(-1 == result){
					fprintf(stderr,"Error[10]: Failed to tell server to quit\n");
					return -10;
				}
				proceed = 0;
			}
			else{
				fprintf(stderr, "Error[3]: invalid input.\n"
						"\tExpected day of the week or quit, got '%s'\n"
						"\t[VALID INPUT]: mon/monday, tues/tuesday, etc, or "
						"q/quit", 
						input);
				return -3;
			}
		}
	}
	return 0;
}

