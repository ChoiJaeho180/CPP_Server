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

class Knight {
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
	/*
	 shared_ptr 내부 구조
	   element_type* _Ptr{nullptr}
		 -> 공유되는 실제 객체를 가리킴
	   _Ref_count_base* _Rep{ nullptr } 
		 -> 참조 카운트를 관리하는 구조체나 클래스의 기본 타입;
		 -> 참조 카운트를 추적하는 객체의 포인터

	std::shared_ptr 생성 시
		 -> _Ptr가 리소스를 가리키도록 설정.
		 -> _Rep는 리소스를 관리하는 제어 블록을 가리키도록 설정.
	복사 / 이동 연산 시
		 -> _Ptr는 동일한 리소스를 가리킴.
		 -> _Rep의 참조 카운트가 증가.
	소멸 시
		 -> _Rep의 참조 카운트를 감소.
		 -> 참조 카운트가 0이 되면
	_Ptr가 가리키는 리소스를 해제.
		 -> _Rep 자체도 해제.*/
	shared_ptr<Knight> knight(new Knight());

	/* 생성 방식에 따른 메모리 할당 및 동작 차이
	* 1. shared_ptr<Knight> knight1(new Knight());
	* 작동 방식
	*	-> new Knight()를 호출하여 Knight 객체를 동적으로 힙에 할당
	*	-> 이 객체의 포인터가 shared_ptr 생성자에 전달
	*	-> shared_ptr은 별도의 메모리를 할당하여 "제어 블록"을 생성하고, 참조 카운트 및 삭제자 정보를 저장
	* 메모리 할당
	*	-> 두 번의 메모리 할당이 발생
	*		-> new Knight()로 객체를 위한 메모리 할당
	*		-> 제어 블록을 위한 메모리를 별도로 할당.
	* 단점
	*	-> 객체와 제어 블록이 서로 다른 메모리 영역에 위치하므로 캐시 효율이 떨어질 수 있음
	* 
	* 2. shared_ptr<Knight> knight2 = make_shared<Knight>()
	* 작동 방식
	*	-> std::make_shared는 객체와 제어 블록을 한 번의 메모리 할당으로 생성
	*	-> 내부적으로 std::allocator를 사용하여 객체와 제어 블록이 동일한 메모리 블록에 배치
	* 메모리 할당
	*	-> 한 번의 메모리 할당만 발생.
	*		-> 객체와 제어 블록이 함께 할당되고, 효율적인 메모리 배치를 제공.
	* 장점
	*	-> 메모리 할당이 한번만 발생하여 효율적
	*	-> 객체와 제어 블록이 인접해 있어 캐시 효율이 높아질 가능성이 있음
	*/
	shared_ptr<Knight> knight1(new Knight());
	shared_ptr<Knight> knight2 = make_shared<Knight>();

	weak_ptr<Knight> wpr = knight1;

}
