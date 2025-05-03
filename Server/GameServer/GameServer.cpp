#include "pch.h"
#include "SocketUtils.h"
#include "ThreadManager.h"
#include "Types.h"
#include "Service.h"
#include "Session.h"
#include "GameSession.h"
#include "GameSessionManager.h"
#include "BufferWriter.h"
#include "ServerPacketHandler.h"
#include <tchar.h>

#pragma comment(lib, "Ws2_32.lib")

// 현재 서버의 동작 방식 
//1. iocp 핸들 생성
//2. tcp 소켓 생성
//3. iocp 핸들에 tcp 소켓 이벤트 감지
//4. tcp 소켓 설정하여 클라이언트 연결을 할 수 있도록 설정.
//		-> bind
//		-> listen
//->클라이언트가 연결 이벤트를 감지할 수 있도록 accepEx 미리 호출
//5. thread 생성하여 이벤트 감지
//6. 이벤트 감지된 경우 iocpObject Dispatch로 처리

// 패킷 직렬화
// 


int main()
{

	

	ServerServiceRef service = MakeShared<ServerService>(
		NetAddress(NetAddress(L"127.0.0.1", 7777)),
		MakeShared<IocpCore>(),
		MakeShared<GameSession>,
		100
	);

	ASSERT_CRASH(service->Start());
	
	for (int i = 0; i < 5; i++) {
		GThreadManager->Launch([=]() {
			while (true) {
				service->GetIocpCore()->Dispatch();
			}
			
		});
	}

	char sendData[] = "가";
	char sendData2[] = u8"가";
	WCHAR sendData3[] = L"가";
	TCHAR sendData4[] = _T("가");

	while (true) {
		PKT_S_TEST_WRITE packet = PKT_S_TEST_WRITE(1001, 100, 10);
		PKT_S_TEST_WRITE::BuffsList buffsList = packet.ReserveBuffsList(3);
		buffsList[0] = { 100, 1.5f };
		buffsList[1] = { 200, 2.5f };
		buffsList[2] = { 300, 3.5f };

		PKT_S_TEST_WRITE::BuffsVictimsList victims0 = packet.ReserveBuffsVictimsList(&buffsList[0], 3);
		victims0[0] = 0;
		victims0[1] = 1000;
		victims0[2] = 2000;

		PKT_S_TEST_WRITE::BuffsVictimsList victims1 = packet.ReserveBuffsVictimsList(&buffsList[1], 3);
		victims1[0] = 1;
		

		PKT_S_TEST_WRITE::BuffsVictimsList victims2 = packet.ReserveBuffsVictimsList(&buffsList[2], 3);
		victims2[1] = 1011100;
		victims2[2] = 20222200;


		SendBufferRef sendBufferRef = packet.CloseAndReturn();
		GSessionManager.Broadcast(sendBufferRef);

		this_thread::sleep_for(250ms);
	}

	GThreadManager->Join();
}