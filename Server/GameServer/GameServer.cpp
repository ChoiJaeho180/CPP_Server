#include "pch.h"
#include <iostream>
#include "CorePch.h"
#include "AccountManager.h"
#include "UserManager.h"

#include <atomic>
#include "ThreadManager.h"


#pragma comment(lib, "ws2_32.lib")


int main()
{
	WSAData wsaData;
	if ((WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0) {
		return 0;
	}

	// AF_INET : IPv4
	// SOCK_STREAM : TCP
	SOCKET listenSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (listenSocket == INVALID_SOCKET) {
		int32  errCode = ::WSAGetLastError();
		cout << "Socket ErrorCode :" << errCode << endl;
		return 0;
	}

	// 나의 주소는? (IP주소 + port) -> xx아파트 yy호
	SOCKADDR_IN serverAddr; //IPv4
	::memset(&serverAddr, 0, sizeof(serverAddr));

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = ::htonl(INADDR_ANY);
	serverAddr.sin_port = ::htons(7777); // host to network short

	// 안내원 폰 개통 : 식당의 대표 전화
	if (::bind(listenSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr))) {
		int32  errCode = ::WSAGetLastError();
		cout << "Socket ErrorCode :" << errCode << endl;
		return 0;
	}

	// 영업 시작
	if (::listen(listenSocket, 10)) {
		int32  errCode = ::WSAGetLastError();
		cout << "Socket ErrorCode :" << errCode << endl;
		return 0;
	}

	while (true) {
		SOCKADDR_IN clientAddr; //IPv4
		::memset(&clientAddr, 0, sizeof(clientAddr));
		int32 len = sizeof(clientAddr);
		SOCKET clientSocket = ::accept(listenSocket, (SOCKADDR*)&clientAddr, &len);
		if (clientSocket == INVALID_SOCKET) {
			int32  errCode = ::WSAGetLastError();
			cout << "Socket ErrorCode :" << errCode << endl;
			return 0;
		}

		// 손님 입장
		char ipAddress[16];
		::inet_ntop(AF_INET, &clientAddr, ipAddress, sizeof(ipAddress));
		cout << "Client Connected! IP = " << ipAddress << endl;
	}
	//윈속 종료
	::WSACleanup();
}