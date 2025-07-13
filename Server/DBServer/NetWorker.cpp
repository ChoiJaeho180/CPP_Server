#include "NetWorker.h"
#include "Types.h"
#include "Service.h"
#include "GameServerSession.h"

void NetWorker::Run(const ServerServiceRef& service)
{
	while (true) {
		// ��Ʈ��ũ �� ��� ó�� -> �ΰ��� ���� ó�� (��Ŷ �ڵ鷯�� ����)
		service->GetIocpCore()->Dispatch(10);
		
		// todo. ó������ �������� �۽��� ��Ŷ �����Ͱ� Ŀ�� ���
		// ����, �۽� ������ �и� ���
		FlushSendQueue(service);
	}
}

void NetWorker::FlushSendQueue(const ServerServiceRef& service)
{
	Queue<SendBufferRef> copy;

	{
		WRITE_LOCK;
		if (_packets.empty())
			return;

		swap(copy, _packets);
	}

	while (copy.empty() == false) {
		SendBufferRef buffer = copy.front();
		copy.pop();
		GameServerSession::GetInstance()->Send(buffer);
	}
}

void NetWorker::EnqueuePacket(SendBufferRef pkt)
{
	WRITE_LOCK;
	_packets.push(pkt);
}
