#pragma once

/*
	BaseAllocator
*/
class BaseAllocator
{
public:
	// �����ؼ� ����ϴ� ���� : �޸� ���̳� ��ź��� �ϱ� ����.
	static void*	Alloc(int32 size);
	static void		Release(void* ptr);
};

