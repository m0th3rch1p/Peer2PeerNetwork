#ifndef ROUTE_H
#define ROUTE_H

#include "HTTPRequest.h"
#include "HTTPServer.h"
struct Route {
	int methods[9];
	char* uri;
	char* (*route_function)(struct HTTPRequest* request, struct HTTPServer* server);
};
#endif