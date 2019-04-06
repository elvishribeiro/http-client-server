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

request_t analyzeRequest(char* request, size_t request_size){

	request_t r;
	int num_request_lines, aux;
	sds *request_lines = sdssplitlen(request, request_size, "\r\n", 2, &num_request_lines);			//see sds documentation
	sds *tokens = sdssplitlen(request_lines[0], sdslen(request_lines[0]), " ", 1, &aux);


	sds path = sdsnew(source_path);
	path = sdscat(path, tokens[1]);
	char *a;
	while ((a = strstr(path, "%20")) != NULL){
		a[0] = ' ';
		for (int i = 1; i < sdslen(path); i++)
			a[i] = a[i+2];
	}
	r.status = (char*) malloc(40*sizeof(char));

	//checks the request
	if (strcmp(tokens[0], "GET")== 0){
		r.method = GET;

		if (doesFileExists(path)){
			r.path = path;

			if (strcmp(tokens[2],"HTTP/1.1") == 0){
				strcpy(r.status,OK);
			}
			else
				strcpy(r.status,HTTP_VER_INC);

		}else{
			strcpy(r.status,NOT_FOUND);
		}

	}else
		strcpy(r.status,BAD_REQUEST);

	//if the requests leads to an error, the new requested file becames the respective error html file

	if (atoi(r.status) != 200){
		sdsfree(path);
		path = sdsnew("samples/");
		char aux[6]; 
		sprintf(aux, "%d", atoi(r.status));

		path = sdscat(path, aux);
		path = sdscat(path, ".html");
		r.path = path;
	}else
	return r;
}