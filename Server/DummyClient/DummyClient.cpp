#include "pch.h"

#include <iostream>
#pragma comment(lib, "ws2_32.lib")


int main()
{
	this_thread::sleep_for(2s);
	// winSock 초기화(ws2_32 라이브러리 초기화)
	// 관련 정보가  wsaData에 채워짐
	WSAData wsaData;
	if ((WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0) {
		return 0;
	}

	SOCKET clientSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (clientSocket == INVALID_SOCKET) {
		return 0;
	}

	u_long on = 1;
	if (::ioctlsocket(clientSocket, FIONBIO, &on) == INVALID_SOCKET) {
		return 0;
	}

	SOCKADDR_IN serverAddr;
	::memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	::inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);
	serverAddr.sin_port = ::htons(7777);

	while (true) {
		if (::connect(clientSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
			if (::WSAGetLastError() == WSAEWOULDBLOCK) {
				continue;
			}

			// 이미 연결된 상태라면 break;
			if (::WSAGetLastError() == WSAEISCONN) {
				break;
			}

			// error
			break;
		}
	}

	cout << "Connected to Server" << endl;
	char sendBuff[100] = "Hello World!";
	while (true) {
		if (::send(clientSocket, sendBuff, sizeof(sendBuff), 0) == SOCKET_ERROR) {
			if (::WSAGetLastError() == WSAEWOULDBLOCK) {
				continue;
			}

			// error
			break;
		}

		cout << "Send Data! Len = " << sizeof(sendBuff) << endl;

		while (true) {
			char recvBuffer[1000];
			int32 recvLen = ::recv(clientSocket, recvBuffer, sizeof(recvBuffer), 0);
			if (recvLen == SOCKET_ERROR) {
				if (::WSAGetLastError() == WSAEWOULDBLOCK) {
					continue;
				}

				// error
				break;
			}
			else if (recvLen == 0) {
				// 연결 끊김
				break;
			}
			
			cout << "Recv Data Len = " << recvLen << endl;
			break;
		}

		this_thread::sleep_for(1s);

	}
	// 소켓 리소스 반환
	::closesocket(clientSocket);

	//윈속 종료
	::WSACleanup();
}
