#include "pch.h"
#include <iostream>
#include "CorePch.h"
#include "AccountManager.h"
#include "UserManager.h"
#include <windows.h>
#include <atomic>
#include "ThreadManager.h"
#include "GameServer.h"
#include "PlayerManager.h"
#include "AccountManager.h"
#include "RefCounting.h"
#include "Memory.h"
#include "Allocator.h"
#include "LockFreeStack.h"

DECLSPEC_ALIGN(16)
class Data {
public:
	SListEntry _entry;
	int64 _rand;

	Data() : _rand(rand() % 100) {}
	~Data() {};
};

SListHeader* GHeader;

int main() {
	GHeader = new SListHeader();
	ASSERT_CRASH(((uint64)GHeader % 16) == 0);
	InitalizeHead(GHeader);


	for (int32 i = 0; i < 3; i++) {
		GThreadManager->Launch([]() 
			{
				while (true) {
					Data* data = new Data();
					ASSERT_CRASH(((uint64)data % 16) == 0);

					PushEntrySList(GHeader, (SListEntry*)data);
					this_thread::sleep_for(10ms);
				}
			});
	}

	for (int32 i = 0; i < 2; i++) {
		GThreadManager->Launch([]()
			{
				while (true) {
					Data* pop = nullptr;
					pop = (Data*)PopEntrySList(GHeader);
					if (pop) {
						cout << pop->_rand << endl;
						delete pop;
					}
					else {
						cout << "NONE" << endl;
					}
				}
			});
	}

	GThreadManager->Join();
}
