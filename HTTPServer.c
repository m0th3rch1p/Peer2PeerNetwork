#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <io.h>
#endif

#include "ThreadPool.h"
#include "HTTPServer.h"
#include "HTTPRequest.h"
#include "HttpResponse.h"
#include "Route.h"

struct ClientServer {
#ifdef _WIN32
	SOCKET client_socket;
#else
	int client_socket;
#endif
	struct HTTPServer* server;
};

static const char* contenTypesStr[] = { "application/json", "text/html" };

char* fetch_file(char* path, int* file_size);
char* render_template(char* base_path, int no_files, ...);
void http_launch_sequence(int noOfThreads, struct HTTPServer* httpServer);
void register_route(void* (route_function)(struct HTTPRequest* request, struct HTTPServer* server), struct HTTPServer* server, char* uri,int no_methods, ...);

struct HTTPServer* http_server_constructor(int addressFamily, int socketType, int protocol, char* host, char* port, int backLog) {
	struct HTTPServer* http_server = (struct HTTPServer*)calloc(1, sizeof(struct HTTPServer));
	if (!http_server) {
		fprintf(stderr, "[-] error allocating memory for http server\n");
		return NULL;
	}
	
	http_server->routes = dictionary_constructor(dict_str_cmp);
	if (!http_server->routes) return NULL;

	http_server->server = server_constructor(addressFamily, socketType, protocol, host, port, backLog);
	if (!http_server->server) {
		fprintf(stderr, "[-] error initializing server....\n");
		free(http_server);
		return NULL;
	}

	http_server->register_route = register_route;
	http_server->launch = http_launch_sequence;
	return http_server;
}

void http_server_deconstructor(struct HTTPServer* http_server) {
	server_deconstructor(http_server->server);
	free(http_server);
}

void* handle_client(struct ClientServer* clientServer) {
	char request[1024] = { 0 };

	//Get Request From Client
	int bytes_read = recv(clientServer->client_socket, request, 1024, NULL);
	printf("\n%s\n", request);

	struct HTTPRequest* http_request = http_request_constructor(request);
	if (http_request) {
		char* route_uri = (char*)((struct DictionaryNode*)http_request->request_line->search("uri", http_request->request_line))->val;

		struct Route* route = (struct Route*)clientServer->server->routes->search(route_uri, clientServer->server->routes)->val;

		char* body_template = route->route_function(http_request, clientServer->server);

		struct HttpResponse* http_response = http_response_constructor();
		char* response;
		if (!body_template) {
			char* headers[] = {
				"Content-Type: text/html",
				"Content-Length: 9"
			};
			response = http_response->generate_response(404, "NOT FOUND", headers, http_response);
		}
		else {
			http_response->body = body_template;
			char bodyLen[50] = { 0 };
			snprintf(bodyLen, sizeof(bodyLen), "%d", (int)strlen(http_response->body));

			char contentLength[50] = { 0 };
			strncpy_s(contentLength, sizeof(contentLength), "Content-Length: ", (sizeof(contentLength) - strlen("Content-Length: ")));
			strncat_s(contentLength, sizeof(contentLength), bodyLen, sizeof(contentLength) - strlen(contentLength));

			char* headers[] = {
				"Content-Type: text/html",
				contentLength,
			};
			response = http_response->generate_response(200, "OK", headers, http_response);
		}

		int bytes_written = send(clientServer->client_socket, response, (int)strlen(response), NULL);

		free(response);
		http_response_deconstructor(http_response);
	}
#ifdef WIN32
	closesocket(clientServer->client_socket);
	Sleep(10);
#else
	close(clientServer->client_socket);
#endif

}

void http_launch_sequence(int noOfThreads, struct HTTPServer* httpServer) {
	struct ClientServer* client_server = (struct ClientServer*)malloc(sizeof(struct ClientServer));
	if (!client_server) {
		fprintf(stderr, "[-] error allocating memory for client server\n");
		exit(1);
	}
	client_server->server = httpServer;
	client_server->client_socket = 0;

	struct ThreadPool* thread_pool = thread_pool_constructor(20);
	
	while (1) {
		client_server->client_socket = accept(httpServer->server->socket, NULL, NULL);

		struct ThreadJob threadJob = thread_job_constructor(handle_client, client_server);
		thread_pool->addJob(threadJob, thread_pool);
	
		Sleep(10);
	}
	thread_pool_deconstructor(thread_pool);
}

char* fetch_file(char* path, int* file_size) {
	FILE* file;

	errno_t error = fopen_s(&file, path, "r");
	if (error != 0) {
		fprintf(stderr, "[-] error fetching path from: %s with error: %d\n", path, GetLastError());
		return NULL;
	}

	fseek(file, 0, SEEK_END);
	long fileSize = ftell(file);
	fseek(file, 0, SEEK_SET);

	char* file_buffer = (char*)calloc(1, fileSize);
	if (!file_buffer) {
		fprintf(stderr, "[-] error allocating file buffer.\n");
		fclose(file);
		return NULL;
	}

	*file_size = fileSize;

	fread(file_buffer, 1, fileSize, file);
	fclose(file);
	return file_buffer;
}

char* render_template(char* base_path, int no_files, ...) {
	va_list file_paths;
	va_start(file_paths, no_files);

	char* rendered_files = (char*)calloc(1, 20000);
	if (!rendered_files) {
		fprintf(stderr, "[-] error allocating memory for rendered_files:\n");
		return NULL;
	}
	
	int fileSize = 0;
		
	char* p = rendered_files;
	for (int i = 0; i < no_files; i++)
	{
		char* path = va_arg(file_paths, char*);
		char* file_buffer = fetch_file(path, &fileSize);
		if (!file_buffer) {
			free(rendered_files);
			return NULL;
		}

		memcpy(p, file_buffer, fileSize);
		p += fileSize;

		free(file_buffer);
	}
	return rendered_files;
}

void register_route(void* (route_function)(struct HTTPRequest* request, struct HTTPServer* server), struct HTTPServer* server, char* uri, int no_methods, ...) {
	struct Route route = { 0 };
	if (no_methods > 0) {
		va_list methods;
		va_start(methods, no_methods);

		for (int i = 0; i < no_methods; i++)
		{
			route.methods[i] = va_arg(methods, int);
		}
	}
	
	route.uri = uri;
	route.route_function = route_function;
	server->routes->insert(uri, strlen(uri) + 1, &route, sizeof(route), server->routes);
}