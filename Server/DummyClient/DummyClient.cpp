#include "pch.h"
#include <iostream>
#include "Service.h"
#include "Session.h"
#include "ThreadManager.h"
#include "BufferReader.h"
#include "ServerPacketHandler.h"

#pragma comment(lib, "ws2_32.lib")

char sendData[] = "Hello world!";

class ServerSession : public PacketSession {
public:
	~ServerSession() {
		cout << "~ServerSession" << endl;
	}

	virtual void OnConnected() override
	{
		// 인증 서버에 로그인 과정을 거쳐야하지만 스킵
		Protocol::C_LOGIN pkt;
		auto sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);
		Send(sendBuffer);
		
	}

	virtual void OnDisconnected() override
	{
		cout << "Disconnected" << endl;
	}

	virtual void OnRecvPacket(BYTE* buffer, int32 len) override {

		std::cout << "ThreadId : " << LThreadId << std::endl;
		/*PacketSessionRef session = GetPacketSessionRef();
		PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);

		ServerPacketHandler::HandlePacket(session, buffer, len);*/

		/*Protocol::C_LOGIN pkt;
		auto sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);
		Send(sendBuffer);*/
	}

	virtual void OnSend(int32 len) override {
	
	}
};

int main()
{
	ServerPacketHandler::Init();

	this_thread::sleep_for(1s);

	ClientServiceRef service = MakeShared<ClientService>(
		NetAddress(L"127.0.0.1", 7777),
		MakeShared<IocpCore>(),
		MakeShared<ServerSession>,
		1);

	ASSERT_CRASH(service->Start());

	for (int i = 0; i < 5; i++) {
		GThreadManager->Launch([=]() {

			while (true) {
				service->GetIocpCore()->Dispatch();
			}
		});
	}
	GThreadManager->Join();
}
