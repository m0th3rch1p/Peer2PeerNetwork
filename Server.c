#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <WinSock2.h>
#include <WS2tcpip.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#endif

#include "Server.h"

struct Server* server_constructor(int addressFamily, int socketType, int protocol, char* host, char* port, int backLog) {
	struct Server* server = (struct Server*)calloc(1, sizeof(struct Server));
	if (!server) {
		fprintf(stderr, "[-] error allocating memory for server\n");
		return NULL;
	}

#ifdef _WIN32
	WSADATA wsaData = { 0 };
	if (WSAStartup(MAKEWORD(2, 2), &wsaData)) {
		fprintf(stderr, "[-] WSAStartup failed\n");
		return NULL;
	}
#endif

	server->socket = socket(addressFamily, socketType, protocol);
	if (server->socket < 0) {
		fprintf(stderr, "[-] error initializing socket.\n");
		free(server);
		return NULL;
	}

	struct addrinfo hints = { 0 };
	hints.ai_family = addressFamily;
	hints.ai_socktype = socketType;
	
	struct addrinfo* addr = 0;
	int addr_ret = getaddrinfo(host, port, &hints, &addr);
	if (addr_ret) {
		fprintf(stderr, "[-] error getting host addr info: %s\n", gai_strerrorA(addr_ret));
		free(server);
		return NULL;
	}

	if (bind(server->socket, addr->ai_addr, addr->ai_addrlen)) {
		freeaddrinfo(addr);
		free(server);
		return NULL;
	}

	freeaddrinfo(addr);
	if (listen(server->socket, backLog)) {
		fprintf(stderr, "[-] error listening for incoming clients.\n");
		free(server);
		return NULL;
	}

	printf("[+] server initialized successfully. waiting for connections....\n");
	return server;
}

void server_deconstructor(struct Server* server) {
	free(server);
}