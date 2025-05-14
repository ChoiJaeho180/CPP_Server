#pragma once
#include "Task.h"

class Room
{
public:
	// 싱글 스레드 환경처럼 개발
	void				Enter(PlayerRef player);
	void				Leave(PlayerRef player);
	void				BroadCast(SendBufferRef sendBuffer);
public:
	// 멀티쓰레드 환경에서는 일감으로 접근
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
