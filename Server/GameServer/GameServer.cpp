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
#include "Protocol.pb.h"

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
		Protocol::S_TEST pkt;
		pkt.set_id(1000);
		pkt.set_hp(100);
		pkt.set_attack(10);
		
		{
			Protocol::BuffData* data = pkt.add_buffs();
			data->set_buffid(100);
			data->set_remaintime(10.f);
			data->add_victims(10);
		}
		
		{
			Protocol::BuffData* data = pkt.add_buffs();
			data->set_buffid(500);
			data->set_remaintime(50.f);
			data->add_victims(50);
			data->add_victims(500);
		}
		SendBufferRef sendBufferRef = ServerPacketHandler::MakeSendBuffer(pkt);
		GSessionManager.Broadcast(sendBufferRef);

		this_thread::sleep_for(250ms);
	}

	GThreadManager->Join();
}