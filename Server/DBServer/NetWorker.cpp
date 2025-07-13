#include "NetWorker.h"
#include "Types.h"
#include "Service.h"
#include "GameServerSession.h"

void NetWorker::Run(const ServerServiceRef& service)
{
	while (true) {
		// 네트워크 입 출력 처리 -> 인게임 로직 처리 (패킷 핸들러에 의해)
		service->GetIocpCore()->Dispatch(10);
		
		// todo. 처리량이 많아지고 송신할 패킷 데이터가 커질 경우
		// 수신, 송신 스레드 분리 고려
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
