#include <iostream>
#include <fstream>

#include <cstdio>

#include <string>
#include <sstream>

#include <algorithm> 
#include <cctype>
#include <locale>

// TODO get rid of this
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <winsock2.h>
#pragma comment(lib,"ws2_32.lib") 
#include <ws2tcpip.h>

static WSADATA wsa;
static SOCKET server_socket;
static sockaddr_in server, client;

#define PORT 49200
#define BUFLEN 61

inline static void rtrim(std::string &s) {
	s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
		return !std::isspace(ch);
		}).base(), s.end());
}

std::string iso_8859_1_to_utf8(std::string &str)
{
	std::stringstream strStr;

	for (std::string::iterator it = str.begin(); it != str.end(); ++it)
	{
		uint8_t ch = *it;
		if (ch < 0x80) {
			strStr << ch;
		}
		else {
			strStr << static_cast<uint8_t>(0xc0 | ch >> 6);
			strStr << static_cast<uint8_t>(0x80 | (ch & 0x3f));
		}
	}

	return strStr.str();
}

int main()
{
	SetConsoleOutputCP(CP_UTF8);

	// version info
	std::cout << "Pokazka kěrlušow V0.9.0" << std::endl;
	std::cout << "=======================" << std::endl;

	// initialise winsock
	std::cout << "Initialising Winsock...\n";
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		std::cout << "Failed. Error Code: " << WSAGetLastError() << "\n";
		exit(EXIT_FAILURE);
	}
	std::cout << "Initialised.\n";

	// create a socket
	if ((server_socket = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET) {
		std::cout << "Could not create socket: " << WSAGetLastError() << "\n";
		exit(EXIT_FAILURE);
	}
	std::cout << "Socket created.\n";

	// prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(PORT);

	// bind
	if (bind(server_socket, (sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
		std::cout << "Bind failed with error code: " << WSAGetLastError() << "\n";
		exit(EXIT_FAILURE);
	}
	std::cout << "Bind done.\n";

	while (true)
	{
		char message[BUFLEN + 1] = {0};

		// try to receive some data, this is a blocking call
		int message_len;
		int slen = sizeof(sockaddr_in);
		if ((message_len = recvfrom(server_socket, message, BUFLEN, 0, (sockaddr*) &client, &slen)) == SOCKET_ERROR) {
			std::cout << "recvfrom() failed with error code: " << WSAGetLastError() << "\n";
			exit(0);
		}

		// print details of the client/peer and the data received
		std::cout << "Received packet from " << inet_ntoa(client.sin_addr) << " " << ntohs(client.sin_port) << "\n";
		// TODO take care of zero-termination!
		std::cout << "Data: " << message << "\n";

		// try to judge overall validity of message
		if ((message[0] == 0x31) || (message[0] == 0x30))
		{
			bool onOff = (message[0] == 0x31) ? true : false;

			for (int index = 0; index < 5; index++)
			{
				int msgBegin = (index * 12) + 1;
				int secondColon = msgBegin + 6;

				// check colons at expected positions
				if ((message[msgBegin] == 0x2C) && (message[secondColon] == 0x2C))
				{
					std::string oneSong(&message[msgBegin + 1], 5);
					std::string verses(&message[secondColon + 1], 5);

					rtrim(oneSong);
					rtrim(verses);

					std::string oneSongUTF8 = iso_8859_1_to_utf8(oneSong);
					std::string versesUTF8 = iso_8859_1_to_utf8(verses);

					// std::string tmpFileName = std::to_string(index + 1) + ".txt.tmp";
					std::string fileName = std::to_string(index + 1) + ".txt";

					if (onOff == true)
					{
						std::cout << "Pisam dataju z kěrlušom '" << oneSongUTF8 << "' a stučkami '" << versesUTF8
									<< "' do dataju '" << fileName << "'." << std::endl;
					}
					else
					{
						std::cout << "Wumazam wobsah dataje '" << fileName << "'." << std::endl;
					}

					// write to temp file
					std::ofstream file;
					file.open(fileName, std::ios::out | std::ios::trunc);
					if (onOff == true)
					{
#if 0
						// write one line only, with space between kěrluš and stučki
						file << oneSong << " " << verses << std::endl;
#endif
						// separate kěrlus and stučki with new line
						// unconditionally pad with spaces (1 before, 3 after)
						file << " " << oneSongUTF8 << "   " << std::endl << " " << versesUTF8 << "   ";
					}
					else
					{
						file << "" << std::endl;
					}
					file.close();

					// move to correct file (may fail, we don't care)
					// std::rename(tmpFileName.c_str(), fileName.c_str());
				}
				else
				{
					std::cout << "Spodźiwna powěsć! Njewočakowane ličby tam hdźež sym wočakował komje." << std::endl;
				}
			}
		}
		else
		{
			std::cout << "Spodźiwna powěsć! Prěnja ličba njeje 0x31 (zaswěčić) ani 0x30 (hasnyć)." << std::endl;
		}
	}

	closesocket(server_socket);
	WSACleanup();
}
