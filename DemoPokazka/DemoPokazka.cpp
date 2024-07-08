#include <iostream>
#include <fstream>
#include <string>

#include <chrono>
#include <thread>

#include <vector>

#include <cassert>

#include <winsock2.h>
#pragma comment(lib,"ws2_32.lib") 
#include <ws2tcpip.h>

static WSADATA ws;
static SOCKET client_socket;
static sockaddr_in server;

#define SERVER "127.0.0.1"
#define PORT 49200

int main()
{
	SetConsoleOutputCP(CP_UTF8);

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
	// server.sin_addr.S_un.S_addr = inet_addr(SERVER);
	InetPton(AF_INET, __TEXT("127.0.0.1"), &server.sin_addr.s_addr);

	std::cout << "Sćelu demo-daty!\n";

	while(1)
	{
		std::ifstream demodata("demodata.txt");
		if (demodata.is_open())
		{
			std::string oneline;
			while (std::getline(demodata, oneline))
			{

				if (oneline.find("#") == 0)
				{
					std::cout << oneline << '\n';
					// std::cout << "Comment line ignored!" << std::endl;
				}
				else
				{
					if (oneline.length() == 182)
					{
						// std::cout << "Expect a UDP packet of fixed size!" << std::endl;
						const char *cstr = oneline.c_str();
						size_t cstr_len = oneline.length();
						size_t index = 0;
						std::vector<uint8_t> udpBytes;
						while (index < cstr_len)
						{
							if (cstr[index] == ' ')
							{
								index++;
							}
							else
							{
								uint16_t oneByte;
								sscanf_s(cstr + index, "%2hX", &oneByte);
								udpBytes.push_back((uint8_t)oneByte);
								index += 2;
							}
						}
						assert(udpBytes.size() == 61);

						udpBytes.push_back(0);

						char udpBytesReadable[62];
						std::copy(udpBytes.begin(), udpBytes.end(), udpBytesReadable);

						std::cout << "UDP: '" << udpBytesReadable << "'" << std::endl;

						// send the message
						if (sendto(client_socket, udpBytesReadable, 61, 0, (sockaddr*)&server, sizeof(sockaddr_in)) == SOCKET_ERROR) {
							std::cout << "sendto() failed with error code: " << WSAGetLastError() << "\n";
							// exit(EXIT_FAILURE);
						}
					}
					else
					{
						int sleepVal = std::stoi(oneline);
						std::cout << "Sleeping " << sleepVal << " ms!" << std::endl;
						std::this_thread::sleep_for(std::chrono::milliseconds(sleepVal));
					}
				}
			}
			demodata.close();
		}
		else
		{
			std::cout << "Njemóžu dataju 'demodata.txt' wočinić!" << std::endl;
		}
	}

	closesocket(client_socket);
	WSACleanup();

}
