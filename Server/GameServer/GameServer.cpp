#include "pch.h"
#include <iostream>
#include "CorePch.h"
#include "AccountManager.h"
#include "UserManager.h"
#include <windows.h>
#include <atomic>
#include "ThreadManager.h"
#include "GameServer.h"

CoreGlobal core;

void ThreadMain() {
	while (true) {
		cout << "Hello! I am thread ..." << LThreadId << endl;
		this_thread::sleep_for(1s);
	}
}

int main() {

	for (int32 i = 0; i < 5; i++) {
		GThreadManager->Launch(ThreadMain);
	}
	GThreadManager->Join();

}
