#include "pch.h"
#include "PacketWorker.h"
#include "DBClientPacketHandler.h"
#include "GameServerSession.h"

void PacketWorker::Tick()
{
	{
		// 1. Session이 연결될 때 까지 대기.
		std::unique_lock<std::mutex> lock(_cvMutex);
		_cv.wait(lock, [&]() { return _isReady && GameServerSession::GetInstance(); });
	}

	cout << "PacketWorker ticking" << endl;
	while (true) {

		{
			// 2. 처리할 패킷이 도착할 때까지 대기
			std::unique_lock<std::mutex> lock(_cvMutex);
			_cv.wait(lock, [&]() {
					READ_LOCK;
					return !_packetsByTargetId.empty();
				});
		}

		HashMap<uint64, Queue<DBServerPacketRef>> copyData;
		{
			// 3. 실제 처리 전에 패킷 큐를 복사
			// 최소한의 시간만 exclusive 락을 잡기 위함 (락 범위 최소화)
			// 이후 복사된 데이터만으로 처리하므로 처리 중에도 Enqueue 가능
			WRITE_LOCK;

			if (_packetsByTargetId.empty())
				continue;

			std::swap(copyData, _packetsByTargetId);
		}

		// 4. 복사본 패킷 처리 (락 없음)
		for (auto& [playerId, queue] : copyData)
		{
			while (queue.empty() == false)
			{
				
				DBServerPacketRef pkt = queue.front();
				cout << "Thread Id : " << LThreadId << ", pkt Id : " << pkt->header.id << endl;
				queue.pop();
				
				DBClientPacketHandler::HandlePacket(GameServerSession::GetInstance(), pkt);
				//DBClientPacketHandler::HandlePacket()
				// 처리
				// ...
			}
		}
	}
}

void PacketWorker::EnqueuePacket(DBServerPacketRef pkt)
{
	{
		WRITE_LOCK;
		_packetsByTargetId[pkt->header.targetId].push(pkt);
	}

	_cv.notify_one(); // 대기 중인 스레드 깨움
}

void PacketWorker::NotifyReady()
{
	{
		std::unique_lock<std::mutex> lock(_cvMutex);
		_isReady = true;
	}

	_cv.notify_one();
}
