#pragma once

/*
	BaseAllocator
*/
class BaseAllocator
{
public:
	// 랩핑해서 사용하는 이유 : 메모리 릭이나 디거빙을 하기 위함.
	static void*	Alloc(int32 size);
	static void		Release(void* ptr);
};

/*
	stompAllocator : 경계 초과(overflow 위주로 잡음), 해제 후 접근 등의 문제를 조기에 감지. 
*/
class StompAllocator
{
	enum { PAGE_SIZE = 0x1000 };
public:
	static void*		Alloc(int32 size);
	static void			Release(void* ptr);
};

/*
	PoolAllocator 
*/
class PoolAllocator
{
	
public:
	static void*		Alloc(int32 size);
	static void			Release(void* ptr);
};


/*
	stlAllocator : 
*/
template<typename T>
class StlAllocator
{
public:
	using value_type = T;

	StlAllocator() {}
	
	template<typename Other>
	StlAllocator(const StlAllocator<Other&>) {}
	
	T* allocator(size_t count) {
		const int32 size = static_cast<int32>(count * sizeof(T));
		return static_cast<T*>(dalloc(size));
	}

	void deallocate(T* ptr, size_t count) {
		drelease(ptr);
	}

};



