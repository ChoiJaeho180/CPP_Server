#include "pch.h"
#include "DBWorker.h"
#include "Service.h"
#include "DBServerSession.h"

DBWorker::DBWorker(int shardId)
	:_shardId(shardId)
{
	cout << "DBWorker::DBWorker shardId : " << _shardId << endl;
}
void DBWorker::AddTask(SendBufferRef task)
{
	_tasks.Push(std::move(task));
	//cout << "AddTask :" << endl;
}

void DBWorker::Run(ClientServiceRef& service)
{
	cout << "DBWorker::Run shardId: " << _shardId << endl;

	while (true)
	{	
		service->GetIocpCore()->Dispatch(10);
		
		// temp
		if (DBServerSession::GetInstance() == nullptr) {
			continue;
		}

		Vector<SendBufferRef> sendBuffers;
		_tasks.PopAll(OUT sendBuffers);
		if (sendBuffers.size() == 0) {
			continue;
		}

		// todo. �ϳ��� Send�� �� �ֵ��� ���� ������ ����.
		for (int i = 0; i < sendBuffers.size(); i++) {
			DBServerSession::GetInstance()->Send(sendBuffers[i]);
			cout << "handle SendBuffer  shardId : " << _shardId << endl;
		}

	}
}