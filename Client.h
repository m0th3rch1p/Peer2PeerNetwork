#pragma once

#ifndef CLIENT_H
#define CLIENT_H

struct Client {
#ifdef _WIN32
	SOCKET socket;
#else
	int socket;
#endif
	int domain;
	int port;
	u_long sock_interface;

	void (*request)(char* server_ip, char* request, struct Client* client);
};

struct Client* client_constructor(int domain, int port, u_long sock_interface);
void client_deconstructor(struct Client* client);
#endif