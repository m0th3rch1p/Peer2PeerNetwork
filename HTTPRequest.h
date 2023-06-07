#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H
#include "Dictionary.h"

struct HTTPRequest {
	struct Dictionary* request_line;
	struct Dictionary* headers;
	struct Dictionary* body;
};

enum ContentType {
	ApplicationJSON,
	TextHTML,
	TransferEncoding
};

struct HTTPRequest* http_request_constructor(char* request);
void http_request_deconstructor(struct HTTPRequest* request);
#endif