#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include "Dictionary.h"
#include "Server.h"

struct HTTPServer {
	struct Server* server;
	struct Dictionary* routes;

	void (*register_route)(void (*route_function)(struct HTTPRequest* request, struct HTTPServer* server), struct HTTPServer* server, char* uri, int no_methods, ...);
	void (*launch)(int noOfThreads, struct HTTPServer* httpServer);
};

struct HTTPServer* http_server_constructor(int addressFamily, int socketType, int protocol, char* host, char* port, int backLog);
void http_server_deconstructor(struct HTTPServer* server);

char* render_template(char* base_path, int no_files, ...);
#endif