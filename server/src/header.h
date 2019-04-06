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
 
// Server side C/C++ program to demonstrate Socket programming 
#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <arpa/inet.h>
#include <errno.h>
#include <sys/stat.h>
#include "sds-master/sds.h"
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <signal.h>
#include <dirent.h>

#define PORT 8080

#define GET 1
#define OK "200 OK"
#define BAD_REQUEST "400 Bad Request"
#define HTTP_VER_INC "505 HTTP Version Not Supported"
#define NOT_FOUND "404 File Not Found"

#define BOLDRED "\033[1;31m"
#define BOLDGREEN "\033[1;32m"
#define RESET "\033[0m"

char *source_path;

typedef struct request_t{
	int method;
	char *path;
	char *status;
}request_t;

typedef struct answer_t{
	char *heading;
	int heading_size;
	char *data;
	int data_size;
}answer_t;

void *get_in_addr(struct sockaddr *sa);
void intHandler(int dummy) ;

int isDirectory(const char *path);
int doesFileExists(const char *path);

request_t analyzeRequest(char* request, size_t request_size);
answer_t makeAnswer (request_t r);
