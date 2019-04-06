all: 
	gcc src/server.c src/sds-master/sds.c src/request_handler.c src/answer_handler.c -o server
	
ddd:
	gcc -c -g server.c sds-master/sds.c 
	gcc server.o sds.o -o server
	ddd server