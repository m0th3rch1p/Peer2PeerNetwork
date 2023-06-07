#pragma once

#include <stdio.h>
#ifdef _WIN32
#include <WinSock2.h>
#include <WS2tcpip.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#include "Client.h"

void request(char* server_ip, char* request, struct Client* client);

struct Client* client_constructor(int domain, int service, int protocol, int port, u_long sock_interface) {
	struct Client* client = (struct Client*)calloc(1, sizeof(struct Client));
	if (!client) {
		fprintf(stderr, "[-] error allocating memory for client in client constructor.\n");
		return NULL;
	}
	
	client->port = port;
	client->domain = domain;
	client->sock_interface = sock_interface;
	client->socket = socket(client->domain, service, protocol);
	client->request = request;

	return client;
}

void client_deconstructor(struct Client* client) {
	free(client);
}

void request(char* server_ip, char* request, struct Client* client) {
	struct sockaddr_in server_address = { 0 };
	server_address.sin_family = client->domain;
	server_address.sin_port = htons(client->port);
	server_address.sin_addr.s_addr = client->sock_interface;

	inet_pton(client->domain, server_ip, &server_address.sin_addr);
	connect(client->socket, (struct sockaddr*)&server_address, sizeof(server_address));

	send(client->socket, request, strlen(request), 0);
}