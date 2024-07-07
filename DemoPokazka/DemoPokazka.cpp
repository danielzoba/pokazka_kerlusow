#include <iostream>

#include <winsock2.h>
#pragma comment(lib,"ws2_32.lib") 

static WSADATA ws;
static SOCKET client_socket;
static sockaddr_in server;

#define SERVER "127.0.0.1"
#define PORT 49200

int main()
{
	std::cout << "Winsock init...\n";
	if (WSAStartup(MAKEWORD(2, 2), &ws) != 0) {
		std::cout << "Failed. Error Code: " << WSAGetLastError() << "\n";
		exit(EXIT_FAILURE);
	}
	std::cout << "Init ok.\n";

	// create socket
	if ((client_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR) {
		std::cout << "socket() failed with error code: " << WSAGetLastError() << "\n";
		exit(EXIT_FAILURE);
	}

	// setup address structure
	memset((char*)&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(PORT);
	server.sin_addr.S_un.S_addr = inet_addr(SERVER);


    std::cout << "Sćelu demo-daty!\n";

	closesocket(client_socket);
	WSACleanup();

}
