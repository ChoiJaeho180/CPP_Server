#include "pch.h"
#include "SocketUtils.h"
#include "ThreadManager.h"

#pragma comment(lib, "Ws2_32.lib")

int main()
{
	SOCKET socket = SocketUtils::CreateSocket();
	SocketUtils::BindAnyAddress(socket, 7777);

	SocketUtils::Listen(socket);

	SOCKET clientSocket = ::accept(socket, NULL, NULL);

	cout << "Client Connected" << endl;

	while (true) {

	}

	GThreadManager->Join();
}