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

		// c++ 특성 상 ++header를 했을 때
		// [memoryHeader][Data] Data의 시작 주소를 반환함. 사용자는 data의 존재만 앎
		return reinterpret_cast<void*>(++header);
	}

	static MemoryHeader* DetachHeader(void* ptr) {
		MemoryHeader* header = reinterpret_cast<MemoryHeader*>(ptr) - 1;
		return header;
	}

	int32 allocSize;
	// TODO : 필요한 추가 정보.
};

// 특정 범위 별로 분류하여 MemoryPool에 담음.

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

