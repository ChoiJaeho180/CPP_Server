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
class Knight {
public:
	void test(int32 b) {};
};
void HealByValue(int64 target, int32 value) {
	cout << target << " 한테 힐을 " << value << "만큼 줌" << endl;
}

int main()
{

	// TEST TASK 
	{
		FuncTask<void, int64, int32> task(HealByValue, 100, 40);
		task.Execute();
	}

	{
		Knight k1;
		MemberTask task(&k1, &Knight::test, 10);
		task.Execute();
	}

	// TASk
	ClientPacketHandler::Init();

	ServerServiceRef service = MakeShared<ServerService>(
		NetAddress(NetAddress(L"127.0.0.1", 7777)),
		MakeShared<IocpCore>(),
		MakeShared<ClientSession>,
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

	while (true) {

		GRoom.FlushTask();

		this_thread::sleep_for(1s);
	}
	GThreadManager->Join();
}