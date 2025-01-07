#include "pch.h"
#include "Memory.h"
#include "MemoryPool.h"

MemoryManager::MemoryManager()
{
	int size = 0;
	int tableIndex = 0;
	for (size = 32; size <= 1024; size += 32) {
		MemoryPool* pool = new MemoryPool(size);
		_pools.push_back(pool);

		while (tableIndex <= size) {
			_poolTable[tableIndex] = pool;
			tableIndex++;
		}
	}

	for (; size <= 2048; size += 128) {
		MemoryPool* pool = new MemoryPool(size);
		_pools.push_back(pool);

		while (tableIndex <= size) {
			_poolTable[tableIndex] = pool;
			tableIndex++;
		}
	}

	for (; size <= 4096; size += 256) {
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

	if (allocSize > MAX_ALLOC_SIZE) {
		header = reinterpret_cast<MemoryHeader*>(::malloc(allocSize));
	}
	else {
		header = _poolTable[allocSize]->Pop();
	}

	
	return MemoryHeader::attachHeader(header,size);
}

void MemoryManager::Release(void* ptr)
{
	MemoryHeader* header = MemoryHeader::DetachHeader(ptr);
	const int32 allocSize = header->allocSize;
	if (allocSize > MAX_ALLOC_SIZE) {
		::free(header);
	}
	else {
		_poolTable[allocSize]->Push(header);
	}
}
