#include "pch.h"

#include <iostream>
#pragma comment(lib, "ws2_32.lib")


int main()
{
	// winSock 초기화(ws2_32 라이브러리 초기화)
	// 관련 정보가  wsaData에 채워짐
	WSAData wsaData;
	if ((WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0) {
		return 0;
	}
	// AF_INET : IPv4
	// SOCK_STREAM : TCP
	SOCKET clientSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (clientSocket == INVALID_SOCKET) {
		int32  errCode = ::WSAGetLastError();
		cout << "Socket ErrorCode :" << errCode << endl;
		return 0;
	}

	SOCKADDR_IN serverAddr; //IPv4
	::memset(&serverAddr, 0, sizeof(serverAddr));
	
	serverAddr.sin_family = AF_INET;
	::inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);
	serverAddr.sin_port = ::htons(7777); // host to network short

	if (::connect(clientSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr))) {
		int32  errCode = ::WSAGetLastError();
		cout << "Socket ErrorCode :" << errCode << endl;
		return 0;
	}

	cout << "connected To Server!" << endl;

	while (true) {
		this_thread::sleep_for(1s);
	}
	
	// 소켓 리소스 반환
	::closesocket(clientSocket);

	//윈속 종료
	::WSACleanup();
}
