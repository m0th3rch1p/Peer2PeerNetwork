#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "HttpResponse.h"

char* generate_response(int response_code, char* response_message, char* custom_headers[], struct HttpResponse* http_response);
char* generate_response_headers(char* custom_headers[]);

struct HttpResponse* http_response_constructor() {
	struct HttpResponse* http_response = (struct HttpResponse*)calloc(1, sizeof(struct HttpResponse));
	if (!http_response) {
		fprintf(stderr, "[-] error allocating response memory\n");
		return NULL;
	}
	
	http_response->generate_response = generate_response;
	return http_response;
};

void http_response_deconstructor(struct HttpResponse* http_response) {
	if (http_response->body) free(http_response->body);
	free(http_response);
}

char* generate_response(int response_code, char* response_message, char* custom_headers[], struct HttpResponse* http_response) {
	char* resp_skeleton = 
		"HTTP/1.1 %d %s\r\n" // Response Header Line
		"%s\r\n\r\n" // Response Headers
		"%s"; // Response Body		

	char* response_headers = generate_response_headers(custom_headers);
	if (!response_headers) return NULL;

	int response_size = strlen(resp_skeleton) + strlen(response_headers) + sizeof(response_code) + sizeof(response_message);
	response_size += http_response->body ? strlen(http_response->body) : strlen("");

	char* response = (char*)calloc(1, response_size);
	if (!response) {
		fprintf(stderr, "[-] error allocating http response\n");
		free(response_headers);
		return NULL;
	}

	snprintf(response, response_size, resp_skeleton, response_code, response_message, response_headers, http_response->body ? http_response->body : "");
	return response;
}		

char* generate_response_headers(char* custom_headers[]) {
	int headers_size = 1024;
	char* headers = (char*)calloc(1, headers_size);
	if (!headers) {
		fprintf(stderr, "[-] error allocating response headers\n");
		return NULL;
	}

	/* pseudo header helper pointers */
	char* p = headers;
	char* end = headers + headers_size;

	/*
		General Response Headers
	*/
	char* server_name_header = "Server: 25420 Warazi Wazimwe\r\n";
	memcpy(p, server_name_header, strlen(server_name_header));
	p += strlen(server_name_header);

	time_t t = 0;
	char* server_response_time_header[100] = { 0 };
	if (ctime_s(server_response_time_header, 100, &t)) {
		fprintf(stderr, "[-] error getting server response time\n");
		free(headers);
		return NULL;
	}

	memcpy(p, server_response_time_header, strlen(server_response_time_header));
	p += strlen(server_response_time_header);

	if (custom_headers != NULL) {
		/*
			Add Custom Headers
		*/
		int no_custom_headers = sizeof(custom_headers) / sizeof(char*);
		for (int i = 0; i < no_custom_headers; i++)
		{
			if ((end - p) < (strlen(custom_headers[i]) + strlen("\r\n"))) {
				// Reallocate and increase header size
				int old_size = headers_size;
				headers_size *= 2;
				headers = (char *)realloc(headers, headers_size);
				if (!headers) {
					fprintf(stderr, "[-] error allocating headers memory\n");
					return NULL;
				}

				p = headers + old_size;
				end = headers + headers_size;
			}
		
			memcpy(p, custom_headers[i], strlen(custom_headers[i]));
			p += strlen(custom_headers[i]);
		
			memcpy(p, "\r\n", strlen("\r\n"));
		}
	}
	return headers;
}