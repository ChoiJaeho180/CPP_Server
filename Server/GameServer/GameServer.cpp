#include "pch.h"
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

	// Select 모델 = (select 함수가 핵심이 됨)
	// 소켓 함수 호출이 성공할 시점을 미리 알 수 있다.
	// 기존 문제 상황)
	// 수신 버퍼에 데이터가 없는데, read 하거나
	// 송신 버퍼에 데이터가 꽉 찼는데, write 할때
	// - 블로킹 소켓 : 조건이 만족되지 않아서 블로킹되는 상황 예방
	// - 논 블로킹 소켓 : 조건이 만족하지 않아서 불필요하게 반복 체크하는 상황 예방.
	
	// socket set
	// 1) 읽기, 쓰기, 예외 관찰 대상 등록
	// 2) select(readSet, writeSet, exceptSet)
	vector<Session> sessions;
	sessions.reserve(100);
	
	fd_set reads;
	fd_set writes;
	while (true) {
		// 매 틱마다 초기화하는 이유
		// select 함수 호출 시 입출력 가능한 소켓만 남도록 변경되기 때문.
		FD_ZERO(&reads);
		FD_ZERO(&writes);

		//ListenSocket 등록
		FD_SET(listenSocket, &reads);
		// 소켓 등록
		for (Session& s : sessions) {
			if (s.recvBytes <= s.sendBytes) {
				FD_SET(s.socket, &reads);
			}
			else {
				FD_SET(s.socket, &writes);
			}
		}


		int retVal = ::select(0, &reads, &writes, nullptr, nullptr);
		if (retVal == SOCKET_ERROR) {
			break;
		}

		//Listener 소켓 체크.
		if (FD_ISSET(listenSocket, &reads)) {
			SOCKADDR_IN clientAddr;
			int32 addrLen = sizeof(clientAddr);
			SOCKET clientSocket = ::accept(listenSocket, (SOCKADDR*)&clientAddr, &addrLen);
			if (clientSocket != INVALID_SOCKET) {
				cout << "Client Connected" << endl;
				sessions.push_back(Session{ clientSocket });
			}
		}

		for (Session& s : sessions) {
			// read
			if (FD_ISSET(s.socket, &reads)) {
				int32 recvLen = ::recv(s.socket, s.recvBuffer, BUF_SIZE, 0);
				if (recvLen <= 0) {
					// TODO : sessions 제거
					continue;
				}
				s.recvBytes = recvLen;
			}

			if (FD_ISSET(s.socket, &writes)) {
				int32 sendLen = ::send(s.socket, &s.recvBuffer[s.sendBytes], s.recvBytes - s.sendBytes, 0);
				if (sendLen == SOCKET_ERROR) {
					// TODO : sessions 제거
					continue;
				}
				s.sendBytes += sendLen;
				if (s.sendBytes == s.recvBytes) {
					s.sendBytes = 0;
					s.recvBytes = 0;
				}
			}
		}
	}
	
	//윈속 종료
	::WSACleanup();
}