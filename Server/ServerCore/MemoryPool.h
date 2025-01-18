#pragma once

enum
{
	SLIST_ALIGNMENT = 16
};

DECLSPEC_ALIGN(SLIST_ALIGNMENT)
struct MemoryHeader : public SLIST_ENTRY
{
	//[memoryHeader][Data]

	MemoryHeader(int32 size): allocSize(size) {}

	static void* attachHeader(MemoryHeader* header, int32 size) {
		new(header)MemoryHeader(size); // placement new

		// c++ Ư�� �� ++header�� ���� ��
		// [memoryHeader][Data] Data�� ���� �ּҸ� ��ȯ��. ����ڴ� data�� ���縸 ��
		return reinterpret_cast<void*>(++header);
	}

	static MemoryHeader* DetachHeader(void* ptr) {
		MemoryHeader* header = reinterpret_cast<MemoryHeader*>(ptr) - 1;
		return header;
	}

	int32 allocSize;
	// TODO : �ʿ��� �߰� ����.
};

// Ư�� ���� ���� �з��Ͽ� MemoryPool�� ����.

DECLSPEC_ALIGN(SLIST_ALIGNMENT)
class MemoryPool
{
public:
	MemoryPool(int32 allocSize);
	~MemoryPool();

	void				Push(MemoryHeader* ptr);
	MemoryHeader*		Pop();
private:
	SLIST_HEADER _header;
	int32 _allocSize = 0;
	atomic<int32> _allocCount = 0;
};

