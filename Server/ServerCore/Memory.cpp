#include "pch.h"
#include "Memory.h"
#include "MemoryPool.h"

MemoryManager::MemoryManager()
{
	int tableIndex = 0;

	for (int size = 32; size <= 1024; size += 32) {
		MemoryPool* pool = new MemoryPool(size);
		_pools.push_back(pool);

		while (tableIndex <= size) {
			_poolTable[tableIndex] = pool;
			tableIndex++;
		}
	}

	for (int size = 1024; size <= 2048; size += 128) {
		MemoryPool* pool = new MemoryPool(size);
		_pools.push_back(pool);

		while (tableIndex <= size) {
			_poolTable[tableIndex] = pool;
			tableIndex++;
		}
	}

	for (int size = 2048; size <= 4096; size += 256) {
		MemoryPool* pool = new MemoryPool(size);
		_pools.push_back(pool);

		while (tableIndex <= size) {
			_poolTable[tableIndex] = pool;
			tableIndex++;
		}
	}

}

MemoryManager::~MemoryManager()
{
	for (MemoryPool* pool : _pools) {
		delete pool;
	}

	_pools.clear();
}

void* MemoryManager::Allocate(int32 size)
{
	MemoryHeader* header = nullptr;
	const int32 allocSize = size + sizeof(MemoryHeader);
#ifdef _STOMP
	header = reinterpret_cast<MemoryHeader*>(StompAllocator::Alloc(allocSize));
#else
	if (allocSize > MAX_ALLOC_SIZE) {
		header = reinterpret_cast<MemoryHeader*>(::_aligned_malloc(allocSize, SLIST_ALIGNMENT));
	}
	else {
		header = _poolTable[allocSize]->Pop();
	}

#endif

	return MemoryHeader::attachHeader(header, allocSize);
}

void MemoryManager::Release(void* ptr)
{
	MemoryHeader* header = MemoryHeader::DetachHeader(ptr);
	const int32 allocSize = header->allocSize;

#ifdef  _STOMP
	StompAllocator::Release(ptr);
#else
	if (allocSize > MAX_ALLOC_SIZE) {
		::_aligned_free(header);
}
	else {
		_poolTable[allocSize]->Push(header);
	}
#endif
}
