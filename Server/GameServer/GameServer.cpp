﻿#include "pch.h"
#include <iostream>
#include "CorePch.h"
#include "AccountManager.h"
#include "UserManager.h"

#include <atomic>
#include "ThreadManager.h"


#pragma comment(lib, "ws2_32.lib")

const int32 BUF_SIZE = 1000;
struct Session {
	SOCKET socket = INVALID_SOCKET;
	char recvBuffer[BUF_SIZE] = {};
	int32 recvBytes = 0;
	int32 sendBytes = 0;
};
int main()
{
	WSAData wsaData;
	if ((WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0) {
		return 0;
	}

	SOCKET listenSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (listenSocket == INVALID_SOCKET) {
		return 0;
	}
	
	u_long on = 1;
	if (::ioctlsocket(listenSocket, FIONBIO, &on) == INVALID_SOCKET) {
		return 0;
	}

	SOCKADDR_IN serverAddr;
	::memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = ::htonl(INADDR_ANY);
	serverAddr.sin_port = ::htons(7777);

	if (::bind(listenSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
		return 0;
	}

	if (::listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
		return 0;
	}

	cout << "Accept" << endl;

	// WSAEventSelect = (WSAEventSelect 함수가 핵심이 됨)
	// 소켓과 관련된 네트워크 이벤트를 [이벤트 객체]를 통해 감지

	vector<WSAEVENT> wsaEvents;
	vector<Session> sessions;
	sessions.reserve(100);
	

	WSAEVENT listenEvent = ::WSACreateEvent();
	wsaEvents.push_back(listenEvent);
	sessions.push_back(Session{ listenSocket });
	if (::WSAEventSelect(listenSocket, listenEvent, FD_ACCEPT | FD_CLOSE) == SOCKET_ERROR) {
		return 0;
	}

	while (true) {
		int32 index = ::WSAWaitForMultipleEvents(wsaEvents.size(), &wsaEvents[0], false, WSA_INFINITE, false);
		if (index == WSA_WAIT_FAILED) {
			continue;
		}
		
		index -= WSA_WAIT_EVENT_0;


		WSANETWORKEVENTS networkEvents;
		if (::WSAEnumNetworkEvents(sessions[index].socket, wsaEvents[index], &networkEvents) == SOCKET_ERROR) {
			continue;
		}

		if (networkEvents.lNetworkEvents & FD_ACCEPT) {
			if (networkEvents.iErrorCode[FD_ACCEPT_BIT] != 0) {
				continue;
			}

			SOCKADDR_IN clientAddr;
			int32 addrLen = sizeof(clientAddr);

			SOCKET clientSocket = ::accept(listenSocket, (SOCKADDR*)&clientAddr, &addrLen);
			if (clientSocket != INVALID_SOCKET) {
				cout << "Client Connected" << endl;
				WSAEVENT clientEvent = ::WSACreateEvent();
				wsaEvents.push_back(clientEvent);
				sessions.push_back(Session{ clientSocket });
				if (::WSAEventSelect(clientSocket, clientEvent, FD_READ | FD_WRITE | FD_CLOSE) == SOCKET_ERROR) {
					return 0;
				}

			}
		}

		// Client Session 소켓 체크
		if (networkEvents.lNetworkEvents & FD_READ || networkEvents.lNetworkEvents & FD_WRITE) {
			if ((networkEvents.lNetworkEvents & FD_READ) && (networkEvents.iErrorCode[FD_READ_BIT] != 0)) {
				continue;
			}

			if ((networkEvents.lNetworkEvents & FD_WRITE) && (networkEvents.iErrorCode[FD_WRITE_BIT] != 0)) {
				continue;
			}

			Session& s = sessions[index];
			if (s.recvBytes == 0) {
				int32 recvLen = ::recv(s.socket, s.recvBuffer, BUF_SIZE, 0);
				if (recvLen == SOCKET_ERROR && ::WSAGetLastError() != WSAEWOULDBLOCK) {
					// TODO: remove session
					continue;
				}
				s.recvBytes = recvLen;
				cout << "Recv Data = " << recvLen << endl;
			}

			if (s.recvBytes > s.sendBytes) {
				int32 sendLen = ::send(s.socket, &s.recvBuffer[s.sendBytes], s.recvBytes - s.sendBytes, 0);
				if (sendLen == SOCKET_ERROR && ::WSAGetLastError() != WSAEWOULDBLOCK) {
					// TODO: remove session
					continue;
				}

				s.sendBytes += sendLen;
				if (s.recvBytes == s.sendBytes) {
					s.recvBytes = 0;
					s.sendBytes = 0;
				}
				cout << "sendData : " <<  sendLen << endl;
			}
		}
		if (networkEvents.lNetworkEvents & FD_CLOSE) {
			
		}
	}
	
	//윈속 종료
	::WSACleanup();
}