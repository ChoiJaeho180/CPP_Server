#include "pch.h"
#include "Lock.h"
#include "CoreTLS.h"

void Lock::WriteLock()
{
	// ������ �����尡 �����ϰ� �ִٸ� ������ ����
	const uint32 lockThreadId = (this->_lockFlag.load() & WRITE_THREAD_MASK) >> 16;
	if (lockThreadId == LThreadId) {
		this->_writeCount++;
		return;
	}

	// �ƹ��� ���� �� �����ϰ� ���� ���� ��, �����ؼ� �������� ��´�.
	// ���� = [WWWWWWWW][WWWWWWWW]
	// ���� = [RRRRRRRR][RRRRRRRR]
	const int64 beginTick = ::GetTickCount64();
	const uint32 desired = ((LThreadId << 16) & WRITE_THREAD_MASK);
	while (true) {
		for (uint32 spinCount = 0; spinCount < MAX_SPIN_COUNT; spinCount++) {
			uint32 expected  = EMPTY_FLAG;
			if (this->_lockFlag.compare_exchange_strong(OUT expected, desired)) {
				this->_writeCount++;
				return;
			}
		}

		const int64 curTick = ::GetTickCount64();
		if (curTick - beginTick >= ACQUIRE_TIMEOUT_TICK) {
			CRASH("LOCK_TIMEOUT");
		}

		this_thread::yield();
	}
}

void Lock::WriteUnlock()
{	
	// ReadLock �� Ǯ�� ������ WriteUnlock �Ұ���
	if ((this->_lockFlag.load() & READ_COUNT_MASK) != 0) {
		CRASH("INVALID_UNLOCK_ORDER");
	}

	const int32 lockCount = --_writeCount;
	if (lockCount == 0) {
		this->_lockFlag.store(EMPTY_FLAG);
	}
}

void Lock::ReadLock()
{
	// ������ �����尡 �����ϰ� �ִٸ� ������ ����
	const uint32 lockThreadId = (this->_lockFlag.load() & WRITE_THREAD_MASK) >> 16;
	if (lockThreadId == LThreadId) {
		this->_lockFlag.fetch_add(1);
		return;
	}

	// �ƹ��� ������ ������ ���� �� �����ؼ� ���� ī��Ʈ�� �ø���.
	const int64 beginTick = ::GetTickCount64();
	while (true) {
		for (uint32 spinCount = 0; spinCount < MAX_SPIN_COUNT; spinCount++) {
			uint32 expected = (this->_lockFlag.load() & READ_COUNT_MASK);
			if (this->_lockFlag.compare_exchange_strong(OUT expected, expected + 1)) {
				return;
			}

		}
		
		const int64 curTick = ::GetTickCount64();
		if (curTick - beginTick >= ACQUIRE_TIMEOUT_TICK) {
			CRASH("LOCK_TIMEOUT");
		}

		this_thread::yield();
	}
}

void Lock::ReadUnlock()
{	
	// fetch_sub�� ���� ���� ���� ��ȯ
	// 0�� ��ȯ�� ��� ������ �ִ� ��Ȳ.
	if ((this->_lockFlag.fetch_sub(1) & READ_COUNT_MASK) == 0) {
		CRASH("MULTIPLX_UNLOCK");
	}

}
