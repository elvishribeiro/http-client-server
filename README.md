# http-client-server
A simple pure C http client and server.
=======

# C HTTP Client-Server for linux

An implementation of the HTTP client and server for the computer networking course.

## Compiling
Both client and server are built by running the Makefile just like this:

```Shell
~/server$ make
```

## Running
### Client
The client will request a file to the HTTP server and save it on a folder named after the server name.
The syntax for running it is:

```Shell
~/client$ ./client hostname:port/requested_file output_file
```
- If the output_file is not specified the client will save it with the name of the file requested to the server.
- If the requested_file is not specified the client will request the root directory of the server.
- If the port is not specified the client will run over the port `80`

## Server
This is a server witch allows simultaneous connections by forking them into child process.
It's syntax is:
```Shell
~server$ ./server directory
```
- If the directory is not specified the server will run on the folder `www`.
- The server runs on the port `8080`

## License
This project was made by Elvis Ribeiro and released by under the GPLv3.
=======
This project was made by Elvis Ribeiro and released by under the GPLv3.

