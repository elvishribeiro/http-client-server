
	/*
** client.c -- a stream socket client demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "sds-master/sds.h"

#include <sys/stat.h>
#include <fcntl.h>

#include <arpa/inet.h>

#define MAXDATASIZE 10000 // max number of bytes we can get at once 

#define BOLDGREEN "\033[1;32m"
#define BOLDRED "\033[1;31m"
#define RESET "\033[0m"

char *port = "80";

void *get_in_addr(struct sockaddr *sa);		// get sockaddr, IPv4 or IPv6:
void parseURL(char *fullURL, char **hostname, char **path);	//parse the URL into hostname and path of file
char *makeRequest(char *hostname, char *path);			//make the html request
int check_package(char *buf, int numbytes);		//returns content_length or -1 if some error


int main(int argc, char *argv[]){
	if (argc < 2) {
		fprintf(stderr,"usage: client hostname outputfile\n");
		exit(1);
	}

	char *hostname, *path, *buf, *syscmd, *request, *output_file_name, *file_init;
	char s[INET6_ADDRSTRLEN];
	int sockfd, numbytes, rv, content_length;
	struct addrinfo hints, *servinfo, *p;
	

	parseURL(argv[1], &hostname, &path);

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo(hostname, port, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	syscmd = (char*) malloc(1024*sizeof(char));
	strcpy(syscmd, "mkdir ");
	strcat(syscmd, hostname);
	system(syscmd);
	
	// loop through all the results and connect to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("client: socket");
			continue;
		}

		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("client: connect");
			continue;
		}

		break;
	}


	if (p == NULL) {
		fprintf(stderr, "client: failed to connect\n");
		return 2;
	}

	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
			s, sizeof s);
	printf("client: connecting to %s\n", s);

	freeaddrinfo(servinfo); // all done with this structure

	request =  makeRequest(hostname, path);
	send(sockfd, request , strlen(request) , 0 ); 		//send the http request
	printf("%s", request);

	//prepare the output file
	if (argc == 2){
		output_file_name = (char*) malloc((strlen(argv[1])+6)*sizeof(char));
		strcpy(output_file_name, hostname);
		if (strstr(path,".") == NULL){
			strcat(output_file_name, "/");
			strcat(output_file_name, "index.html");
		}
		else{
			strcat(output_file_name, path);
		}
		printf("%s\n", output_file_name);
	}else{
		output_file_name = (char*)malloc((strlen(argv[1]) + strlen(argv[2]))*sizeof(char));	//url size + 6 characters for format
		strcpy(output_file_name, hostname);
		strcat(output_file_name, "/");
		strcat(output_file_name, argv[2]);
	}

	FILE *arq;
	if ((arq = fopen(output_file_name, "wb")) == NULL){
		perror(hostname);
		return 0;
	}

	buf = (char *) malloc(MAXDATASIZE*sizeof(char));

	//As the file comes by parts we have to get the first package and then catch the rest
	if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1,0)) == -1) {
			perror("recv");
			exit(1);
		}

	file_init = strstr(buf, "\r\n\r\n")+4*sizeof(char);			
	printf("'%s'\n", buf);
	
	if ((content_length = check_package(buf,numbytes)) ==-1){
		close(sockfd);
		return 0;
	}
	int data_size = numbytes - (file_init - buf);
	fwrite(file_init, sizeof(char), data_size, arq);

	//get the rest of the data
	if (numbytes < content_length)
		do{
			if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1,0)) == -1) {
				perror("recv");
				exit(1);
			}
			if (numbytes <= 0)		//end of the package
				break;
			fwrite(buf, sizeof(char), numbytes, arq);	
<<<<<<< 710e4e3bed48a890b00448d4eb98dd2ac8784cdc
			fputs(buf, arq);
=======
			
>>>>>>> Writing the output bug fixed (was writing twice
		}while (numbytes > 0);

	printf("OK. Arquivo gravado em" BOLDGREEN" %s\n", output_file_name);
	close(sockfd);
    fclose(arq);
	return 0;
}


void *get_in_addr(struct sockaddr *sa){
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

//parse the URL into hostname and path of file
void parseURL(char *fullURL, char **hostname, char **path){	
	if (strstr(fullURL, "http://") != NULL)		//completly removes http:// from the urls
		fullURL = &fullURL[7];
	printf("%s\n", fullURL);
	int hostnameTam =0;
	int pathTam;
	int i = 0;


	while(fullURL[hostnameTam] != '/' && fullURL[hostnameTam] != '\0')
		 hostnameTam++;
	hostnameTam++;

	(*hostname) = (char*) malloc(hostnameTam*sizeof(char));

	//gets the hostname
	while (fullURL[i] != '/' && fullURL[i] != '\0'){
		(*hostname)[i] = fullURL[i++];
	}
	(*hostname)[i] = '\0';

	//if the port is specified
	char *aux;
	if ((aux = strstr((*hostname), ":")) != NULL){
		port = &aux[1];
		aux[0] = '\0';
	}

	//if the path is not specified, the / is required
	pathTam = strlen(fullURL) - hostnameTam +2;
	if (pathTam == 1){
		(*path) = (char*) malloc(pathTam*sizeof(char));
		pathTam = 2;
		(*path)[0] = '/';
		(*path)[1] = '\0';
		return;
	}

	//Gets the path of the file 
	(*path) = (char*) malloc(pathTam*sizeof(char));
	int j = 0;
	while (fullURL[i] != '\0'){
		(*path)[j++] = fullURL[i++];
	}
	(*path)[i] = '\0';			
}

char *makeRequest(char *hostname, char *path){			//make the html request
	char *request = (char*) malloc (1024*sizeof(char*));

	strcpy(request,"GET ");
	strcat(request, path);
	strcat(request, " HTTP/1.1\r\n");
	strcat(request, "Host: ");
	strcat(request, hostname);
	strcat(request, "\r\nConnection: keep-alive\r\nUpgrade-Insecure-Requests: 1\r\nUser-Agent: ElvisClient/5.0 (X11; Linux x86_64)\r\n"
		"Accept: text/html\r\nAccept-Encoding: deflate\r\n"
		"Accept-Language: en-US,en;q=0.9,pt-BR;q=0.8,pt;q=0.7\r\n\r\n");
	
	return request;
	
}

int check_package(char *buf, int numbytes){		//returns content_length or -1 if some error

	if ((strstr(buf, "HTTP/1.1"))!=buf){
		printf(BOLDRED "Server did not sent a HTTP answer.\n" RESET);
		return -1;
	}
	char status_code[4];
	int i;

	for (i = 0; i < 3; i++)		//get the status code
		status_code[i] = buf[i+9];
	status_code[i++] = '\0';

	char status_message[100];
	int j;
	for (j = 0; buf[i+9] !='\r'; i++, j++)		//get the status message
		status_message[j] = buf[i+9];
	status_message[j] = '\0';

	if (atoi(status_code) != 200){
		printf(BOLDRED"%s %s\n"RESET, status_code, status_message);
		return -1;
	}
	char *aux = strstr(buf, "Content-Length");
	char content_length[255];

	for (i = 0; aux[i+16] != '\r'; i++)		//get the status code
		content_length[i] = aux[i+16];
	content_length[i] = '\0';

	return atoi(content_length);
}