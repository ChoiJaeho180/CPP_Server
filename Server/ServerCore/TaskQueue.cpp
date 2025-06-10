#include "pch.h"
#include "TaskQueue.h"


void TaskQueue::Push(TaskRef task, bool bPushOnly)
{
	const uint32 prevCount = _taskCount.fetch_add(1);
	_tasks.Push(task); // WRITE_LOCK;

	// ù ��° task�� �߰��� �����常 Execute()�� ȣ���Ͽ� ó��
	// ��, ���� �����尡 �̹� �ٸ� TaskQueue�� ���� ���̶��,
	// �� TaskQueue�� GGlobalQueue�� ����Ͽ� ���� �ִ� ��Ŀ �����尡 ó���ϰ� �����Ѵ�.
	// �� ��ø�� Execute() ȣ�� ���� + �л� ó�� ����
	if (prevCount == 0) {
		// �̹� �������� TaskQueue�� ������ ����
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

		// ��Ŀ �����忡�� �Ҵ�� �ð� �̻��� �ҿ�� ���
		// ���� TaskQueue�� GlobalQueue�� �ѱ�� ���� ����
		const uint64 now = ::GetTickCount64();
		if (now >= LEndTickCount) {
			LCurrentTaskQueue = nullptr;
			GGlobalQueue->Push(shared_from_this());
			break;
		}
	}
}
