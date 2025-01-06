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
	Vector<Knight> v(100);
		priority_queue<
}
