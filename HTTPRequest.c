#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "HTTPRequest.h"

void extract_headers(char* header_fields, struct HTTPRequest* http_request);
void extract_query_str(char* uri, struct HTTPRequest* http_request);
void extract_json_body(char* body, struct HTTPRequest* http_request);

struct HTTPRequest* http_request_constructor(char* request) {
	struct HTTPRequest* http_request = (struct HTTPRequest*)malloc(sizeof(struct HTTPRequest));
	if (!http_request) {
		fprintf(stderr, "[-] error allocating memory for http request\n");
		return NULL;
	}

	http_request->request_line = dictionary_constructor(dict_str_cmp);
	if (!http_request->request_line) return NULL;

	http_request->headers = dictionary_constructor(dict_str_cmp);
	if (!http_request->headers) return NULL;
	
	http_request->body = dictionary_constructor(dict_str_cmp);
	if (!http_request->body) return NULL;

	char* req_line_end = strstr(request, "\r\n");
	if (req_line_end == NULL) {
		fprintf(stderr, "[-] request seems broken");
		return NULL;
	}

	*req_line_end = '\0';

	char* request_line = request;
	
	req_line_end += 2;
	char* hdr_fields_end = strstr(req_line_end, "\r\n\r\n");
	if (hdr_fields_end) *hdr_fields_end = '\0';
	char* header_fields = req_line_end;

	char* body = NULL;
	if(hdr_fields_end) body = hdr_fields_end + 2;


	//size_t req_len = sizeof(request_line);
	char* next_tkn = { 0 };
	char* method = strtok_s(request_line, " ", &next_tkn);
	char* uri = strtok_s(NULL, " ", &next_tkn);

	strtok_s(NULL, "/", &next_tkn);
	char* http_version = strtok_s(NULL, "/", &next_tkn);

	http_request->request_line->insert("method", (int)strlen("method") + 1, method, (int)strlen(method) + 1, http_request->request_line);
	http_request->request_line->insert("uri", (int)strlen("uri") + 1, uri, (int)strlen(uri) + 1, http_request->request_line);
	http_request->request_line->insert("http_version", (int)strlen("http_version") + 1, http_version, (int)strlen(http_version) + 1, http_request->request_line);
	
	strtok_s(NULL, "\r\n", &next_tkn);
	extract_headers(header_fields, http_request);

	char* req_method = (char*)((struct DictionaryNode*)http_request->request_line->search("method", http_request->request_line))->val;

	if (!strcmp(req_method, "GET")) {
		char* req_uri = (char *)((struct DictionaryNode*)http_request->request_line->search("uri", http_request->request_line))->val;
		extract_query_str(uri, http_request);
	}
	else if (!strcmp(req_method, "POST")) {
		/*
			Check Content Type
		*/
		char* contentType = (char *)((struct DictionaryNode*)http_request->headers->search("Content-Type", http_request->headers))->val;
		if (!strcmp(contentType, "application/json")) {
			extract_json_body(body, http_request);
		}
	}
	
	return http_request;
}

void http_request_deconstructor(struct HTTPRequest* http_request) {
	dictionary_deconstructor(http_request->request_line);
	dictionary_deconstructor(http_request->headers);
	dictionary_deconstructor(http_request->body);

	free(http_request);
}

void extract_headers(char* header_fields, struct HTTPRequest* http_request) {
	char* next_tkn;
	char* header = header_fields;
	char* end = header + (int)strlen(header);
	while (header < end) {
		char* header_key_end = strchr(header, ':');
		if (!header_key_end) break;

		*header_key_end = '\0';
		char* header_key = header;

		char* header_val_end = strchr(header_key_end + 2, '\r\n');
		if (!header_val_end) break;

		*header_val_end = '\0';

		header_val_end += 2;
		char* header_val = header_key_end + 2;
		http_request->headers->insert(header_key, (int)strlen(header_key) + 1, header_val, (int)strlen(header_val) + 1, http_request->headers);
		char* header_end = strchr(header_val_end, '\r\n');
		if (header_end) header = header_end + 2;
		else header = end + 1;
	}
}

void extract_query_str(char* uri, struct HTTPRequest* http_request) {
	//Check if uri actually has query parameters
	char* next_tkn;
	if (!strtok_s(uri, "?", &next_tkn)) return;

	char* query = strtok_s(NULL, "&", &next_tkn);
	while (query) {
		char* query_key = strtok_s(NULL, "=", &next_tkn);
		char* query_val = strtok_s(NULL, "=", &next_tkn);

		http_request->body->insert(query_key, (int)strlen(query_key) + 1, query_val, (int)strlen(query_val) + 1, http_request->body);
		query = strtok_s(NULL, "&", &next_tkn);
	}
}

void extract_json_body (char* body, struct HTTPRequest* http_request) {
	char* json_start = strchr(body, '\"');
	char* json_end = strchr(body, '}');
	*json_end = 0;
	
	char* json_elements = json_start++;
	while (json_elements) {
		char* key = strchr(json_elements, ':');
		char* val = strchr(key++, ',');

		/*http_request->body->insert(key, (int)strlen(key), val, (int)strlen(val), http_request->body);
		json_element = strtok_s(NULL, ",", &next_tkn);*/
	}
}