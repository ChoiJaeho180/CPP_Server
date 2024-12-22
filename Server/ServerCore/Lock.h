#pragma once
#include "Types.h"

/*----------------------------------------------------------------------------
* RW SpinLock
----------------------------------------------------------------------------*/

// ǥ�� mutex ������
// 1. ��������� lock�� �� ����. (���� recursive_lock �ڷ����� ���� �ֱ� ��.)


// ������ ����� ����
// 99.9999999999 read�� �ϴµ� lock�� ��� ��ۼ� ���� ������ ������ ����
// ���ϴ� ���·� ����ȭ
// ������� �̿��� ������ �� �ִ� ��� �߰�

/*----------------------------------------------------------------------------
[WWWWWWWW][WWWWWWWW][RRRRRRRR][RRRRRRRR]
W: WriteFlag (Exclusive Lock Owner TrheadId)
R: ReadFlag (Shared Lock Count)
----------------------------------------------------------------------------*/

// W -> R (O)
// R -> W (X, ReadLock�� ��� �ٸ� �����嵵 ��� �ִµ�, Ư�� �����尡 writeLock�� ���� �� ���� ���� )

class Lock
{
	enum : uint32 {
		ACQUIRE_TIMEOUT_TICK = 10000,
		MAX_SPIN_COUNT = 5000,
		WRITE_THREAD_MASK = 0xFFFF'0000, // ���� 16��Ʈ�� �̾ƿ��� ���� mask
		READ_COUNT_MASK = 0x0000'FFFF, // ���� 16��Ʈ�� �̾ƿ��� ���� mask
		EMPTY_FLAG = 0x0000'0000,
	};

public:
	void WriteLock(const char* name);
	void WriteUnlock(const char* name);
	void ReadLock(const char* name);
	void ReadUnlock(const char* name);

private:
	Atomic<uint32> _lockFlag = EMPTY_FLAG;

	// ���� �����ϴ� ����
	// -> ��������� ȣ���Ͽ����� writeCount�� �����ϵ��� ó���ϱ� ����
	// lock�� ���� �ָ� ���������� ����ϱ� ������ atomic���� ������ �ʾƵ� ��. 
	uint16 _writeCount = 0;
};

/*
	LockGuards (RAII ����)
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