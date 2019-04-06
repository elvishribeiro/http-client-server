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

//Gets the current time of the system and converts it to a formated date
char* get_time(){									
	char *buf = (char*) malloc(45*sizeof(char));
	struct timeval now;
	gettimeofday(&now, NULL);
	struct tm *ts;
	ts = localtime(&now.tv_sec);
	strftime(buf, 45, "Date: %a, %d %b %Y %T GMT\r\n", ts);
	return buf;
}


//Creates a index.html listing the files and folders of the current directory
char* makeIndex(char *path){			
	struct dirent *de;
	DIR *dr = opendir(path);
	int root_folder_tam = 0;
	while (path[root_folder_tam++] != '/');
	printf("'''''%d'''''", root_folder_tam);
	sds index = sdsnew("");
	sds file_modified = sdsnewlen("", 100);
	sds file_name = sdsnew("");
	sds file_name_absolute = sdsnew("");


	if (dr == NULL){  // opendir returns NULL if couldn't open directory 
		printf("Could not open current directory" ); 
	}

	index = sdscat(index,   "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 3.2 Final//EN\">\n"
							"<html>\n"
 							"<head>\n"
  							"<title>Index of /</title>\n"
 							"</head>\n"
 							"<body>\n"
							"<h1>Index of /</h1>\n"
  							"<table>\n"
						    "<tr><th valign=\"top\">"
							"<img src=\".icons/blank.gif\" alt=\"[ICO]\">"
							"</th><th>Name</th><th>"
							"Last modified</th><th>"
							"Description</th></tr><tr>"
							"<th colspan=\"5\"><hr></th></tr>\n"
							"<tr><td valign=\"top\"><img src=\"/.icons/back.gif\">"
							"</td><td><a href=\"/\">"
							"Parent Directory</a></td><td>&nbsp;</td>"
							"<td align=\"right\">  - </td><td>&nbsp;</td></tr>");
	struct stat attr;
	while ((de = readdir(dr)) != NULL){
		if (de->d_name[0] == '.')				//skips the hidden files
			continue;
		file_name_absolute = sdsnew(path);
		file_name_absolute = sdscat(file_name_absolute, de->d_name);
		stat(file_name_absolute, &attr);
		//file_name = sdsnew("/");
	
		//file_name = sdscat(file_name, de->d_name);
		file_name = sdsnew(&file_name_absolute[root_folder_tam]);
		if (isDirectory(file_name_absolute)){
			printf("%s diretorio\n", file_name);
			file_name = sdscat(file_name, "/");
		}else
		printf("%s arq\n", file_name);

		strftime(file_modified, 100, "%Y-%m-%d %H:%M", localtime( &attr.st_mtime));

		index = sdscat(index, "<tr><td valign=\"top\">");

		if (isDirectory(file_name_absolute))									//--FIX THIS---tricky and ugly way to tell if a file is a file, works some times
			index = sdscat(index,"<img src=\".icons/folder.gif\" alt=\"[DIR]\">");
		else
			index = sdscat(index, "<img src=\".icons/text.gif\" alt=\"[FILE]\">");
		
		index = sdscat(index,"</td><td><a href=\"");
		index = sdscat(index, file_name);
		index = sdscat(index, "\">");
		index = sdscat(index, de->d_name);
		index = sdscat(index, "</a></td>");
		index = sdscat(index, "<td align=\"right\">");
		index = sdscat(index, file_modified);
		index = sdscat(index, "</td><td align=\"right\">  - </td><td>&nbsp;</td></tr>\n");
	//	sdsfree(file_name);
	} 

	index = sdscat(index, "</table>\n"
						  "<address>Elvis Server/0.2 (Ubuntu) Server at localhost Port 8080</address>\n"
						  "</body></html>\n\0");

	return index;
}

//This function is used in case the requested file is a directory
answer_t makeAnswerDirectory(answer_t a, request_t r){
	if (r.path[strlen(r.path)-1] !='/'){			//if path does not have a bar in the end, concatenate it to the end so it works later
		r.path = sdscat(r.path, "/\0");
	}
	sds aux = sdsnew(r.path);				
	aux = sdscat(aux, "index.html");	

	//tries to open the index.html on the folder path, if index.html doesn't exists it creates one
	FILE *arq = fopen(aux, "r");		 
	if (arq != NULL){
		//basicaly the same thing on the makeAnswer function;

		int i = -1;
		
		while(!feof(arq)){
			fgetc(arq);
			i++;
		}
		a.data_size = i;
		char aux[18];
		sprintf(aux, "%d\r\n", i);
		strcat(a.heading, aux);
		strcat(a.heading,"\r\n\0");

		a.heading_size = strlen(a.heading);
		a.heading = (char*)realloc(a.heading, a.heading_size+1);
		

		fseek(arq, SEEK_SET, 0);
		a.data = (char*) malloc((a.data_size+1)*sizeof(char));
		for (i = 0; i < a.data_size-1; i++){
			a.data[i] = fgetc(arq);
		}
		a.data[i++] = '\n';
		a.data[i] = '\0';
		fclose(arq);
		
		return a;
	}else{
		char *data;
		data = makeIndex(r.path);
		a.data_size = strlen(data);

		char aux[18];
		sprintf(aux, "%d\r\n", a.data_size);	//content-length
		strcat(a.heading, aux);
		strcat(a.heading,"\r\n\0");

		a.heading_size = strlen(a.heading);
		a.heading = (char*)realloc(a.heading, a.heading_size+1);

		a.data = data;
		return a;
	}
}

answer_t makeAnswer (request_t r){
	answer_t a;
	char *str;
	a.heading = (char*) calloc(5000,sizeof(char));
	
	//creating the first lines
	strcpy(a.heading,"HTTP/1.1 ");

	strcat(a.heading, r.status); 
	strcat(a.heading, "\r\n");

	str = get_time();
	strcat(a.heading, str);

	strcat(a.heading, "Server: Elvis Server/0.2 (Ubuntu)\r\n");
	strcat(a.heading, "Vary: Accept-Encoding\r\n");
	strcat(a.heading, "Content-Length: ");

	//checking if the requested file is a directory
	if (isDirectory(r.path))
		return makeAnswerDirectory(a,r);


	FILE *arq = fopen(r.path, "r");
	if (arq == NULL){
		perror("fopen");
		exit(1);
	}
	int i = -1;
	
	//calculating the content-length (I did not realize how to do it more efficiently)
	FILE *teste = fopen("teste.jpg", "w");
	while(!feof(arq)){
		fprintf(teste,"%c", fgetc(arq));
		i++;
	}
	a.data_size = i;
	char aux[18];
	sprintf(aux, "%d\r\n", i);
	strcat(a.heading, aux);
	strcat(a.heading,"\r\n\0");

	a.heading_size = strlen(a.heading);
	a.heading = (char*)realloc(a.heading, a.heading_size+1);
	//---- end of heading 

	//start copying data
	fseek(arq, SEEK_SET, 0);
	a.data = (char*) malloc((a.data_size+1)*sizeof(char));
	for (i = 0; i < a.data_size; i++){
		a.data[i] = fgetc(arq);
	}
	a.data[i++] = '\n';
	a.data[i] = '\0';

	free(str);
	return a;
}