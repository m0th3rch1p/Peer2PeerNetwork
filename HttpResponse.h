#pragma once
#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H

struct HttpResponse {
	int statusCode;
	char* body;

	char* (*generate_response)(int response_code, char* response_messsage, char* custom_headers[], struct HttpResponse* http_response);
};

struct HttpResponse* http_response_constructor();
void http_response_deconstructor(struct HttpResponse* http_response);
#endif