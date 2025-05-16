#pragma once
#include "Task.h"
#include "LockQueue.h"

class TaskQueue : public enable_shared_from_this<TaskQueue>
{
public:
	void DoAsync(CallbackType&& callback) {
		Push(ObjectPool<Task>::MakeShared(std::move(callback)));
	}

	template<typename T, typename Ret, typename... Args>
	void DoAsync(Ret(T::* memFunc)(Args ...), Args... args) {
		weak_ptr<T> owner = static_pointer_cast<T>(shared_from_this());
		Push( ObjectPool<Task>::MakeShared(owner, memFunc, std::forward<Args>(args)...));
	}

private:

	// 락 없이도 단일 객체의 직렬성 보장 + 멀티스레드 안전 + 빠른 반응성
	void						Push(TaskRef&& task);
public:

	void						Execute();
protected:
	LockQueue<TaskRef>		_tasks;
	Atomic<uint32>			_taskCount = 0;
};

