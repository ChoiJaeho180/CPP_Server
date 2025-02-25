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
	WSAOVERLAPPED overlapped = {};
	SOCKET socket = INVALID_SOCKET;
	char recvBuffer[BUF_SIZE] = {};
	int32 recvBytes = 0;
};

void CALLBACK RecvCallback(DWORD error, DWORD recvLen, LPWSAOVERLAPPED overlapped, DWORD flags) {
	cout << "Data Recv Len Callback = " << recvLen << endl;
	Session* session = (Session*)overlapped;
	// TODO : 에코 서버를 만든다면 WSASend()
}
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

	// Overlapped IO(비동기 + 논블로킹)
	//  - Overlapped 함수를 건다 (WSARecv, WSASend)
	//  - Overlapped 함수가 성공했는지 확인 후
	//		-> 성공했으면 결과를 얻어서 처리
	//		-> 실패했으면 사유를 확인.

	// Select 모델
	//	장점 : 윈도우/리눅스 공통
	//  단점 : 성능 최하(매번 등록 비용), 64개 제한
	
	// WSAEventSelect 모델
	//  장점 : 비교적 뛰어난 성능
	//  단점 : 64개 제한
	// 
	// Overlapped (이벤트 기반)
	//  장점 : 성능 
	//  단점 : 64개 제한
	// 
	// Overlapped (콜백 기반)
	//  장점 : 성능
	//  단점 : 모든 비동기 소켓 함수에서 사용 가능하진 않음 (accept), 빈번한 Alertable Wait로 인한 성능 저하.
	//   

	// Reactor Pattern (~뒤늦게, 논블로킹 소켓. 소켓 상태 확인 후 -> 뒤늦게 recv send 호출)
	// Proactor Pattern (~미리. Overlapped WSA)

	while (true) {
		SOCKADDR_IN clientAddr;
		int32 addrLen = sizeof(clientAddr);
		SOCKET clientSocket;
		while (true) {
			clientSocket = ::accept(listenSocket, (SOCKADDR*)&clientAddr, &addrLen);
			if (clientSocket != INVALID_SOCKET) {
				break;
			}

			if (::WSAGetLastError() == WSAEWOULDBLOCK) {
				continue;
			}

			// 문제 있는 상황
			return 0;
		}

		Session session = Session{ clientSocket };
		

		cout << "Client Connected!" << endl;

		while (true) {
			WSABUF wsaBuf;
			wsaBuf.buf = session.recvBuffer;
			wsaBuf.len = BUF_SIZE;

			DWORD recvLen = 0;
			DWORD flags = 0;
			if (::WSARecv(clientSocket, &wsaBuf, 1, &recvLen, &flags, &session.overlapped, RecvCallback) == SOCKET_ERROR){
				if (::WSAGetLastError() == WSA_IO_PENDING) {
					::SleepEx(INFINITE, TRUE);
					//::WSAWaitForMultipleEvents(1, &wsaEvent, TRUE, WSA_INFINITE, TRUE);
					//::WSAGetOverlappedResult(session.socket, &session.overlapped, &recvLen, FALSE, &flags);
				}
				else {
					// TODO : 문제 있는 상황
					break;
				}
			}
			else {
				cout << "Data Recv Len = " << recvLen << endl;
			}

		}

		::closesocket(session.socket);
	}

	//윈속 종료
	::WSACleanup();
}