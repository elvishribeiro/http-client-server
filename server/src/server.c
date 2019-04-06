/* Copyright (C) 2019 Elvis H. Ribeiro <elvishribeiro@gmail.com> 
 * 
 * All rights reserved. 
 *  
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions are met: 
 *  
 * * Redistributions of source code must retain the above copyright notice, 
 *   this list of conditions and the following disclaimer. 
 *  
 * * Redistributions in binary form must reproduce the above copyright notice, 
 *   this list of conditions and the following disclaimer in the documentation 
 *   and/or other materials provided with the distribution. 
 *  
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND 
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR 
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON 
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */

#ifndef HEADER
#define HEADER
#include "header.h"
#endif

static volatile int keepRunning = 1;

void *get_in_addr(struct sockaddr *sa){
	if (sa->sa_family == AF_INET) {

		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void intHandler(int dummy) {
	keepRunning = 0;
	printf("\nShuting down server...\nGoodbye!\n\n");
    sleep(1);
	exit(1);
}


int main(int argc, char *argv[]){

	if (argc == 2 && strcmp(argv[1], "--help") == 0){
			printf("Elvis Server v:0.2\n"
		   "Running the server:\n"
		   	"./client source_path\n\n");
		return 0;
	}

	//if not specified the server will run under www folder
	if (argc == 2)
		source_path = argv[1];
	else
		source_path = "www";

	if (!isDirectory(source_path)){
		printf("%s is either not a directory or does not exist.\n", argv[1]);
		return 0;
	}
	printf("\nStarting server at %s...\n", source_path);
	source_path = argv[1];

	int server_fd, new_socket, numbytes; 
	struct sockaddr_in address; 
	int opt = 1; 
	int addrlen = sizeof(address); 
	request_t r;
	signal(SIGINT, intHandler);
	answer_t a;

	char s[24];
	char buffer[1000] = {0}; 

    printf("Waiting for connections...\n");

	// Creating socket file descriptor 
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0){ 
		perror("socket failed"); 
		exit(EXIT_FAILURE); 
	} 
	   
	// Forcefully attaching socket to the port 8080 
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,&opt, sizeof(opt))){ 
		perror("setsockopt"); 
		exit(EXIT_FAILURE); 
	} 
	address.sin_family = AF_INET; 
	address.sin_addr.s_addr = INADDR_ANY; 
	address.sin_port = htons( PORT ); 
	   
	// Forcefully attaching socket to the port 8080 

	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0){ 
		perror("bind failed"); 
		exit(EXIT_FAILURE); 
	} 
	if (listen(server_fd, 10) < 0){ 
		perror("listen"); 
		exit(EXIT_FAILURE); 
	}
	while(keepRunning){
		if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0){ 
			perror("accept"); 
			continue;
		}

		inet_ntop(address.sin_family, get_in_addr((struct sockaddr *)&address), s, sizeof s);
		if (!fork()) { // this is the child process

			numbytes = read(new_socket , buffer, 1024);
			r = analyzeRequest(buffer, numbytes);

			printf("Receiving connection from: %s\n%s\n\n", s, r.status);
			a = makeAnswer(r);
			
			close(server_fd); // child doesn't need the listener
			if (send(new_socket, a.heading, a.heading_size, 0) == -1)
				perror("send");
			if (send(new_socket, a.data, a.data_size, 0) == -1)
				perror("send");
			
			close(new_socket);
			exit(0);
		}
		close(new_socket);
	}
	return 0;
} 



int isDirectory(const char *path) {
   struct stat statbuf;
   if (stat(path, &statbuf) != 0)
	   return 0;
   return S_ISDIR(statbuf.st_mode);
}

int doesFileExists(const char *path){
	// Check for file existence
	if (access(path, R_OK) == -1)
		return 0;

	return 1;
}


