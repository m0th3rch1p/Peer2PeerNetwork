#ifndef SERVER_H
#define SERVER_H

#ifndef _WIN32
typedef SOCKET unsigned long long;
#else
#include <Windows.h>
// link with Ws2_32.lib
#pragma comment(lib,"Ws2_32.lib")
#endif

struct Server {
	SOCKET socket;
};

struct Server* server_constructor(int addressFamily, int socketType, int protocol, char* host, char* port, int backLog);
void server_deconstructor(struct Server* server);
#endif