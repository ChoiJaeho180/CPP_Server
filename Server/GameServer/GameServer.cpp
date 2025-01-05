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
	/*
	* a 객체를 삭제한 이후에도 접근이 가능한 이유
	* -> 메모리 해제 요청했을 때, 운영체제에 즉시 반환하는 것이 아니라 
	* -> "힙 관리자가 관리하는 메모리 풀에 반환될 가능성이 높음."
	*	-> 힙 관리자는 성능을 높이기 위해 메모리를 즉시 운영 체제에 반환하지 않고, 
	*	-> 다음 요청에 제사용할 수 있도록 내부적으로 관리
	* 정리
	* -> delete 후에도 해당 메모리 주소에 접근하면 여전히 이전 값이 남아 있을 수 있음.
	* 
	* 개선점
	* -> virtualFree 사용 (use-after-free 해소)
	*	-> 메모리 반환 시 즉시 반환되어, 해당 메모리 접근 시 즉각적인 충돌이 발생할 가능성이 높음..
	*/
	Knight* a = new Knight();
	a->_hp = 100;
	delete a;
	a->_hp = 200;

	// 해제 후 접근 개선(내부에 virtualMalloc, virtualFree 사용)
	Knight* knight = xnew<Knight>();
	xdelete<Knight>(knight);
	knight->_hp = 100;

	// 경계 초과 테스트
	Knight* test = (Knight*)xnew<Player>();
	test->_hp = 100;
	xdelete(test);
}
