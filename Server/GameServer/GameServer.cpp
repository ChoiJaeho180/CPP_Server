#include "pch.h"
#include "SocketUtils.h"
#include "ThreadManager.h"
#include "Types.h"
#include "Service.h"
#include "Session.h"
#include "ClientSession.h"
#include "ClientSessionManager.h"
#include "BufferWriter.h"
#include "ClientPacketHandler.h"
#include <tchar.h>
#include "Protocol.pb.h"
#include "Room.h"
#include "Player.h"

#pragma comment(lib, "Ws2_32.lib")

enum {
	// TODO. 런타임에 경과를 지켜보고 보정하도록 수정
	WORKER_TICK = 64
};
void DoWokerJob(ServerServiceRef& service) {

	while (true) {
		
		LEndTickCount = ::GetTickCount64() + WORKER_TICK;

		// 네트워크 입 출력 처리 -> 인게임 로직 처리 (패킷 핸들러에 의해)
		service->GetIocpCore()->Dispatch(10);

		// 예약된 task 처리
		ThreadManager::ProcessReservedTasks();

		// 글로벌 큐 
		ThreadManager::ProcessGlobalQueue();
	}
}


int main()
{
	// TEST TASK 
	GRoom->DoTimer(1000, []() {cout << "1000!!!!" << endl; });
	GRoom->DoTimer(2000, []() {cout << "2000!!!!" << endl; });
	GRoom->DoTimer(3000, []() {cout << "3000!!!" << endl; });
	// TASK
	ClientPacketHandler::Init();

	ServerServiceRef service = MakeShared<ServerService>(
		NetAddress(NetAddress(L"127.0.0.1", 7777)),
		MakeShared<IocpCore>(),
		MakeShared<ClientSession>,
		100
	);

	ASSERT_CRASH(service->Start());
	
	for (int i = 0; i < 5; i++) {
		GThreadManager->Launch([&service]() {
			DoWokerJob(service);
		});
	}
	
	DoWokerJob(service);

	GThreadManager->Join();
}