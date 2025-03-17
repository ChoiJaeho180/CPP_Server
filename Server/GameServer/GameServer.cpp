#include "pch.h"
#include "SocketUtils.h"
#include "ThreadManager.h"
#include "Types.h"
#include "Listener.h"
#include "SocketUtils.h"
#pragma comment(lib, "Ws2_32.lib")

int main()
{
	Listener listener;
	listener.StartAccept(NetAddress(L"127.0.0.1", 7777));
	for (int i = 0; i < 5; i++) {
		GThreadManager->Launch([=]() {
			while (true) {
				GIocpCore.Dispatch();
			}
			
		});
	}

	GThreadManager->Join();
}