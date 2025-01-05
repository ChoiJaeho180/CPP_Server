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

