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

	char sendData[] = "Hello world!";

	while (true) {
		vector<BuffData> buffs{ BuffData{100, 1.5f}, BuffData{200, 2.3f }, BuffData{ 300, 0.7f } };
		SendBufferRef sendBuffRef = ServerPacketHandler::Make_S_TEST(1001, 100, 10, buffs);
		GSessionManager.Broadcast(sendBuffRef);

		this_thread::sleep_for(250ms);
	}

	GThreadManager->Join();
}