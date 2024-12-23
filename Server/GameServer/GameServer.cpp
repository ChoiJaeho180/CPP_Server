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


using KnightRef = TSharedPtr<class Knight>;
using InventoryRef = TSharedPtr<class Inventory>;

class Knight : public RefCountable {
public:
	Knight() { cout << "Knight()" << endl; };
	~Knight() { cout << "~Knight()" << endl; };


public:
	InventoryRef _inventory;
};

class Inventory : public RefCountable {
public:
	Inventory(KnightRef knight): _knight (**knight) { }

	// 참조만 하여 Knight의 reference Count를 증가 시키지 않음.
	Knight& _knight;
};

int main() {
	KnightRef knight1(new Knight());
	knight1->ReleaseRef();
	InventoryRef inven(new Inventory(knight1));
	inven->ReleaseRef();
	knight1->_inventory = inven;

	knight1 = nullptr;
	inven = nullptr;
}
