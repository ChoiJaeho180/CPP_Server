#include "pch.h"
#include <iostream>
#include "Service.h"
#include "Session.h"
#include "ThreadManager.h"
#include "BufferReader.h"
#include "ClientPacketHandler.h"

#pragma comment(lib, "ws2_32.lib")

char sendData[] = "Hello world!";

class ServerSession : public PacketSession {
public:
	~ServerSession() {
		cout << "~ServerSession" << endl;
	}

	virtual void OnConnected() override
	{
	}

	virtual void OnDisconnected() override
	{
		cout << "Disconnected" << endl;
	}

	virtual void OnRecvPacket(BYTE* buffer, int32 len) override {
		ClientPacketHandler::HandlePacket(buffer, len);
		
	}

	virtual void OnSend(int32 len) override {
	
	}
};

int main()
{
	this_thread::sleep_for(1s);

	ClientServiceRef service = MakeShared<ClientService>(
		NetAddress(L"127.0.0.1", 7777),
		MakeShared<IocpCore>(),
		MakeShared<ServerSession>,
		1);

	ASSERT_CRASH(service->Start());

	for (int i = 0; i < 2; i++) {
		GThreadManager->Launch([=]() {
			while (true) {
				service->GetIocpCore()->Dispatch();
			}

			});
	}
	GThreadManager->Join();
}
