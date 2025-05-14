#pragma once
#include "Task.h"

class Room
{
public:
	// �̱� ������ ȯ��ó�� ����
	void				Enter(PlayerRef player);
	void				Leave(PlayerRef player);
	void				BroadCast(SendBufferRef sendBuffer);
public:
	// ��Ƽ������ ȯ�濡���� �ϰ����� ����
	void PushTask(TaskRef task) { _tasks.Push(task); }
	void FlushTask();
	
	template<typename T, typename Ret, typename... Args> 
	void PushTask(Ret(T::* memFunc)(Args...), Args... args) {
		auto task = MakeShared<MemberTask<T, Ret, Args...>>(static_cast<T*>(this), memFunc, args ...);
		_tasks.Push(task);
	}
private:
	map<uint64, PlayerRef>		_players;

	TaskQueue					_tasks;
};

extern Room GRoom;
