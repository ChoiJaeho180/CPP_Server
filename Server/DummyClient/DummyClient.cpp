#include "pch.h"
#include <iostream>
#include "Service.h"
#include "Session.h"
#include "ThreadManager.h"
#include "BufferReader.h"
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

	virtual int32 OnRecvPacket(BYTE* buffer, int32 len) override {
		BufferReader reader(buffer, len);
		reader.Read(buffer);

		PacketHeader header;
		reader >> header;

		uint64 id;
		uint32 hp;
		uint16 attack;
		reader >> id >> hp >> attack;

		cout << " Id : " << id << "hp : " << hp << "att :" << attack << endl;

		char recvBuffer[4096] = {0};
		reader.Read(recvBuffer, header.size - sizeof(PacketHeader) - 8 - 4 - 2);
		cout << recvBuffer << endl;

		return len;
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
		1000);

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
