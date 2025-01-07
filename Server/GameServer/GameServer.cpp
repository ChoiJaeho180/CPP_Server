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

class Player {

};

class Knight: public Player {
public:
	Knight() { cout << "Knight()" << endl; };
	Knight(int32 hp) : _hp(hp) {
		cout << "Knight(hp)" << endl;
	}
	~Knight() { cout << "~Knight()" << endl; };

	//void* operator new(size_t size) {
	//	cout << "Knight new! " << size << endl;
	//	void* ptr = ::malloc(size);
	//	return ptr;
	//}

	//void operator delete(void* ptr) {
	//	cout << "Knight delete!" << endl;
	//	::free(ptr);

	//}
public:
	int _hp;
};

int main() {
	//for (int32 i = 0; i < 5; i++) {
	//	GThreadManager->Launch([]() {
	//		while (true) {
	//			Vector<Knight> v(10);
	//			/*Map<int32, Knight> m;
	//			m[100] = Knight();*/
	//			this_thread::sleep_for(10ms);
	//		}
	//	});

	//}
	//GThreadManager->Join();
}
