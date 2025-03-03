#include "pch.h"
#include <iostream>
#include "CorePch.h"
#include "AccountManager.h"
#include "UserManager.h"

#include <atomic>
#include "ThreadManager.h"
#include "Memory.h"

#pragma comment(lib, "ws2_32.lib")

const int32 BUF_SIZE = 1000;
struct Session {
	SOCKET socket = INVALID_SOCKET;
	char recvBuffer[BUF_SIZE] = {};
	int32 recvBytes = 0;
};

enum IO_TYPE {
	READ,
	WRITE,
	ACCEPT,
	CONNECT
};

// 어떤 타입으로 호출되었는지 위해 선언
struct OverlappedEx {
	WSAOVERLAPPED overlapped = {};
	int32 type = 0; //IO_TYPE
};

void WorkerThreadMain(HANDLE iocpHandle) {
	while (true) {
		DWORD bytesTransferred = 0;
		Session* session = nullptr;
		OverlappedEx* overlappedEx = nullptr;

		BOOL ret = ::GetQueuedCompletionStatus(iocpHandle, &bytesTransferred, (ULONG_PTR*)&session, (LPOVERLAPPED*)&overlappedEx, INFINITE);
		
		if (ret == FALSE || bytesTransferred == 0) {
			// TODO : 연결 끊김
			continue;
		}
		
		ASSERT_CRASH(overlappedEx->type == IO_TYPE::READ);
		cout << "Recv Data IOCP = " << bytesTransferred << endl;

		WSABUF wsaBuf;
		wsaBuf.buf = session->recvBuffer;
		wsaBuf.len = BUF_SIZE;

		DWORD recvLen = 0;
		DWORD flags = 0;

		// 최초 1번 호출해야하는 이유?
		// 커널에 비동기 읽기 요청을 등록하기 위해

		::WSARecv(session->socket, &wsaBuf, 1, &recvLen, &flags, &overlappedEx->overlapped, NULL);
	}
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
	//		   소켓:이벤트 1:1 대응
	// 
	// Overlapped (콜백 기반)
	// - 비동기 입출력 함수 완료되면, 쓰레드마다 있는 APC 큐에 일감이 쌓임
	// - Alertable Wait 상태로 들어가서 APC 큐 비우기 (콜백 함수)
	//  장점 : 성능
	//  단점 : 모든 비동기 소켓 함수에서 사용 가능하진 않음 (accept), 
	//		   빈번한 Alertable Wait로 인한 성능 저하.
	//         APC큐는 쓰레드마다 있기 때문에 멀티 쓰레드 환경에서 일감 분배를 하기 힘들다.
	//   

	// Reactor Pattern (~뒤늦게, 논블로킹 소켓. 소켓 상태 확인 후 -> 뒤늦게 recv send 호출)
	// Proactor Pattern (~미리. Overlapped WSA)
	vector<Session*> sessionManager;
	// 
	HANDLE iocpHandle = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

	for (int i = 0; i < 5; i++) {
		GThreadManager->Launch([=]() {WorkerThreadMain(iocpHandle); });
	}
	while (true) {
		SOCKADDR_IN clientAddr;
		int32 addrLen = sizeof(clientAddr);
		SOCKET clientSocket = ::accept(listenSocket, (SOCKADDR*)&clientAddr, &addrLen);
		if (clientSocket == INVALID_SOCKET) {
			return 0;
		}

		Session* session = xnew<Session>();
		session->socket = clientSocket;
		sessionManager.push_back(session);

		cout << "Client Connected!" << endl;


		// 소켓을 CP에 등록
		::CreateIoCompletionPort((HANDLE)clientSocket, iocpHandle, (ULONG_PTR)session, 0);
		
		WSABUF wsaBuf;
		wsaBuf.buf = session->recvBuffer;
		wsaBuf.len = BUF_SIZE;

		OverlappedEx* overlappedEx = new OverlappedEx();
		overlappedEx->type = IO_TYPE::READ;

		DWORD recvLen = 0;
		DWORD flags = 0;

		// 최초 1번 호출해야하는 이유?
		// 커널에 비동기 읽기 요청을 등록하기 위해
		::WSARecv(clientSocket, &wsaBuf, 1, &recvLen, &flags, &overlappedEx->overlapped, NULL);

		Session* s = sessionManager.back();
		sessionManager.pop_back();
		xdelete(s);
	}

	GThreadManager->Join();

	//윈속 종료
	::WSACleanup();
}