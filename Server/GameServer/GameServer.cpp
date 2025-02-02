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

using TL = TypeList<class Player, class Mage, class Knight, class Archer>;
class Player {
public:
	Player() {
		INIT_TL(Player);
	}
	virtual ~Player() {}

	DECLARE_TL;
};

class Knight: public Player {
public:
	int32 _hp = rand() % 1000;
	Knight() {
		INIT_TL(Knight);
	}

};

class Mage : public Player {
public:
	Mage() {
		INIT_TL(Mage);
	}
};

class Archer : public Player {
public:

	Archer() {
		INIT_TL(Archer);
	}
};


int main() { 
	Player* p1 = new Knight();
	Player* p2 = new Mage();

	Knight* k1 = dynamic_cast<Knight*>(p1);
	// static_cast
	//	-> mage 객체도 knight로 형 변환이 가능하여 메모리 오염이 생길 수 있음
	// dynamic_cast
	//  -> 형 변환이 되지 않을 경우 nullptr를 반환 
	//  -> 속도가 느림


	TypeList<Mage, Knight>::Head whoAmI;
	TypeList<Mage, Knight>::Tail whoAmTail;
	
	int32 len1 = Length<TypeList<Mage, Knight, Archer>>::value;

	bool canConvert = Conversion<Player, Knight>::exists;
	bool canConvert2 = Conversion<Knight, Player>::exists;

	TypeConversion<TL> test;
	auto a = test.s_convert[0];
	auto b = test.s_convert[1];
	{
		Player* player = new Player();
		bool canCast = CanCast<Knight*>(player);
		Knight* knight = TypeCast<Knight*>(player);
		delete player;
	}

	{
		shared_ptr<Knight> knight = MakeShared<Knight>();
		shared_ptr<Player> player = TypeCast<Player>(knight);
	}

	/*for (int32 i = 0; i < 3; i++) {
		GThreadManager->Launch([]() 
			{
				while (true) {
				}
			});
	}

	GThreadManager->Join();*/
}
