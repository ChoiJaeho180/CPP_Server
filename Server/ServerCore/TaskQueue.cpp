#include "pch.h"
#include "TaskQueue.h"


void TaskQueue::Push(TaskRef task, bool bPushOnly)
{
	const uint32 prevCount = _taskCount.fetch_add(1);
	_tasks.Push(task); // WRITE_LOCK;

	// 첫 번째 task를 추가한 스레드만 Execute()를 호출하여 처리
	// 단, 현재 스레드가 이미 다른 TaskQueue를 실행 중이라면,
	// 이 TaskQueue는 GGlobalQueue에 등록하여 여유 있는 워커 스레드가 처리하게 위임한다.
	// → 중첩된 Execute() 호출 방지 + 분산 처리 유도
	if (prevCount == 0) {
		// 이미 실행중인 TaskQueue가 없으면 실행
		if (LCurrentTaskQueue == nullptr && bPushOnly == false) {
			Execute();
		}
		else {
			GGlobalQueue->Push(shared_from_this());
		}
	}
}

void TaskQueue::Execute()
{
	LCurrentTaskQueue = this;

	while (true) {
		Vector<TaskRef> tasks;
		_tasks.PopAll(OUT tasks);

		const uint32 taskCount = static_cast<uint32>(tasks.size());
		for (uint32 i = 0; i < taskCount; i++) {
			tasks[i]->Execute();
		}

		if (taskCount == _taskCount.fetch_sub(taskCount)) {
			LCurrentTaskQueue = nullptr;
			break;
		}

		// 워커 스레드에게 할당된 시간 이상이 소요된 경우
		// 현재 TaskQueue를 GlobalQueue에 넘기고 실행 종료
		const uint64 now = ::GetTickCount64();
		if (now >= LEndTickCount) {
			LCurrentTaskQueue = nullptr;
			GGlobalQueue->Push(shared_from_this());
			break;
		}
	}
}
