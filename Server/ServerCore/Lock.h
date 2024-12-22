#pragma once
#include "Types.h"

/*----------------------------------------------------------------------------
* RW SpinLock
----------------------------------------------------------------------------*/

// 표준 mutex 문제점
// 1. 재귀적으로 lock을 못 잡음. (물론 recursive_lock 자료형이 따로 있긴 함.)


// 별도로 만드는 이유
// 99.9999999999 read를 하는데 lock을 계속 잠글수 없기 떄문에 별도로 만듬
// 원하는 형태로 최적화
// 데드락을 미연에 방지할 수 있는 기능 추가

/*----------------------------------------------------------------------------
[WWWWWWWW][WWWWWWWW][RRRRRRRR][RRRRRRRR]
W: WriteFlag (Exclusive Lock Owner TrheadId)
R: ReadFlag (Shared Lock Count)
----------------------------------------------------------------------------*/

// W -> R (O)
// R -> W (X, ReadLock의 경우 다른 스레드도 잡고 있는데, 특정 쓰레드가 writeLock을 잡을 수 없기 떄문 )

class Lock
{
	enum : uint32 {
		ACQUIRE_TIMEOUT_TICK = 10000,
		MAX_SPIN_COUNT = 5000,
		WRITE_THREAD_MASK = 0xFFFF'0000, // 상위 16비트를 뽑아오기 위한 mask
		READ_COUNT_MASK = 0x0000'FFFF, // 하위 16비트를 뽑아오기 위한 mask
		EMPTY_FLAG = 0x0000'0000,
	};

public:
	void WriteLock(const char* name);
	void WriteUnlock(const char* name);
	void ReadLock(const char* name);
	void ReadUnlock(const char* name);

private:
	Atomic<uint32> _lockFlag = EMPTY_FLAG;

	// 따로 관리하는 이유
	// -> 재귀적으로 호출하였을떄 writeCount만 증가하도록 처리하기 위함
	// lock을 잡은 애만 독단적으로 사용하기 떄문에 atomic으로 만들지 않아도 됨. 
	uint16 _writeCount = 0;
};

/*
	LockGuards (RAII 패턴)
*/
class ReadLockGuard {
public:
	ReadLockGuard(Lock& lock, const char* name) : _lock(lock), _name(name) { _lock.ReadLock(name); }
	~ReadLockGuard() { _lock.ReadUnlock(_name); }
private:
	Lock& _lock;
	const char* _name;
};

class WriteLockGuard {
public:
	WriteLockGuard(Lock& lock, const char* name) : _lock(lock), _name(name) { _lock.WriteLock(_name); }
	~WriteLockGuard() { _lock.WriteUnlock(_name); }
private:
	Lock& _lock;
	const char* _name;
};