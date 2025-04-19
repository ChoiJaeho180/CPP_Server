#include "pch.h"
#include <iostream>
#include "Service.h"
#include "Session.h"
#include "ThreadManager.h"
#pragma comment(lib, "ws2_32.lib")

char sendData[] = "Hello world!";

class ServerSession : public Session {
public:
	~ServerSession() {
		cout << "~ServerSession" << endl;
	}

	virtual void OnConnected() override
	{
		cout << "Connected To Server" << endl;
		SendBufferRef sendBuffRef = GSendBufferManager->Open(4096);
		::memcpy(sendBuffRef->Buffer(), sendData, sizeof(sendData));
		sendBuffRef->Close(sizeof(sendData));
		
		Send(sendBuffRef);
	}

	virtual void OnDisconnected() override
	{
		cout << "Disconnected" << endl;
	}

	virtual int32 OnRecv(BYTE* buffer, int32 len) override {
		cout << "OnRecv Len = " << len << endl;
		this_thread::sleep_for(1s);

		SendBufferRef sendBuffRef = GSendBufferManager->Open(4096);
		::memcpy(sendBuffRef->Buffer(), sendData, sizeof(sendData));
		sendBuffRef->Close(sizeof(sendData));
		Send(sendBuffRef);

		return len;
	}

	virtual void OnSend(int32 len) override {
		cout << "OnSend Len = " << len << endl;
	}
};

int main()
{
	this_thread::sleep_for(1s);

	ClientServiceRef service = MakeShared<ClientService>(
		NetAddress(L"127.0.0.1", 7777),
		MakeShared<IocpCore>(),
		MakeShared<ServerSession>,
		5);

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
