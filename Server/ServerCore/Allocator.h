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

/*
	stompAllocator : ��� �ʰ�(overflow ���ַ� ����), ���� �� ���� ���� ������ ���⿡ ����. 
	
*/
class StompAllocator
{
	enum { PAGE_SIZE = 0x1000 };
public:
	static void*		Alloc(int32 size);
	static void			Release(void* ptr);
};

