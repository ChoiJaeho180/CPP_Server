#pragma once
#include "Task.h"
#include "LockQueue.h"
#include "TaskTimer.h"


// TaskQueue
// 객체 단위 단일 스레드 실행을 보장하면서도,
// 멀티스레드 환경에서 안전하게 Task를 추가하고 처리할 수 있도록 설계된 큐 시스템
// .
// TaskQueue 이점 
// 1. 동시성 제어 없이 안전한 실행 흐름을 보장
//	-> TaskQueue는 여러 스레드에서 동시에 Push 할 수 있다
//  -> 하지만 작업은 순차적으로 하나씩만 실행되고 락 없이도 공유 자원의 일관성을 보장한다
// 2. 한 객체에 대해 단일 스레드 접근을 보장
//  -> Room 내부 상태는 오직 해당 TaskQueue를 통해서만 수정한다.
//  -> 그래서 TaskQueue를 상속 받은 객체는 WRITE_LOCK 없이도 안전하게 설계 가능
// 3. 사용하지 않는 스레드 활용
//  -> 현재 스레드가 시간 제한을 초과하면 나머지 작업을 GGlobalQueue에 넘김
class TaskQueue : public enable_shared_from_this<TaskQueue>
{
public:
	void DoAsync(CallbackType&& callback) {
		Push(ObjectPool<Task>::MakeShared(std::move(callback)));
	}

	// args 보편 참조 일부러 하지 않음
	// 참조하다가 객체가 제거될 경우 떄문
	template<typename T, typename Ret, typename... Args>
	void DoAsync(Ret(T::* memFunc)(Args ...), Args... args) {
		weak_ptr<T> owner = static_pointer_cast<T>(shared_from_this());
		const TaskRef task = ObjectPool<Task>::MakeShared(owner, memFunc, std::forward<Args>(args)...);
		Push(task);
	}

	void Enqueue(CallbackType&& callback) {
		Push(ObjectPool<Task>::MakeShared(std::move(callback)), true);
	}

	template<typename T, typename Ret, typename... Args>
	void Enqueue(Ret(T::* memFunc)(Args ...), Args... args) {
		weak_ptr<T> owner = static_pointer_cast<T>(shared_from_this());
		const TaskRef task = ObjectPool<Task>::MakeShared(owner, memFunc, std::forward<Args>(args)...);
		Push(task, true);
	}
	
	void DoTimer(uint64 tickAfter, CallbackType&& callback) {
		TaskRef task = ObjectPool<Task>::MakeShared(std::move(callback));
		GTaskTimer->Reserve(tickAfter, shared_from_this(), task);
	}

	template<typename T, typename Ret, typename... Args>
	void DoTimer(uint64 tickAfter, Ret(T::* memFunc)(Args ...), Args... args) {
		weak_ptr<T> owner = static_pointer_cast<T>(shared_from_this());
		TaskRef task = ObjectPool<Task>::MakeShared(owner, memFunc, std::forward<Args>(args)...);
		GTaskTimer->Reserve(tickAfter, owner, task);
	}
	
public:
	// 락 없이도 단일 객체의 직렬성 보장 + 멀티스레드 안전 + 빠른 반응성
	void						Push(TaskRef task, bool bPushOnly = false);
	void						Execute();
protected:
	LockQueue<TaskRef>		_tasks;
	Atomic<uint32>			_taskCount = 0;
};

