#include "pch.h"
#include "PacketWorker.h"
#include "DBClientPacketHandler.h"
#include "GameServerSession.h"

void PacketWorker::Tick()
{
	{
		// 1. Session�� ����� �� ���� ���.
		std::unique_lock<std::mutex> lock(_cvMutex);
		_cv.wait(lock, [&]() { return _isReady && GameServerSession::GetInstance(); });
	}

	cout << "PacketWorker ticking" << endl;
	while (true) {

		{
			// 2. ó���� ��Ŷ�� ������ ������ ���
			std::unique_lock<std::mutex> lock(_cvMutex);
			_cv.wait(lock, [&]() {
					READ_LOCK;
					return !_packetsByTargetId.empty();
				});
		}

		HashMap<uint64, Queue<DBServerPacketRef>> copyData;
		{
			// 3. ���� ó�� ���� ��Ŷ ť�� ����
			// �ּ����� �ð��� exclusive ���� ��� ���� (�� ���� �ּ�ȭ)
			// ���� ����� �����͸����� ó���ϹǷ� ó�� �߿��� Enqueue ����
			WRITE_LOCK;

			if (_packetsByTargetId.empty())
				continue;

			std::swap(copyData, _packetsByTargetId);
		}

		// 4. ���纻 ��Ŷ ó�� (�� ����)
		for (auto& [playerId, queue] : copyData)
		{
			while (queue.empty() == false)
			{
				
				DBServerPacketRef pkt = queue.front();
				cout << "Thread Id : " << LThreadId << ", pkt Id : " << pkt->header.id << endl;
				queue.pop();
				
				DBClientPacketHandler::HandlePacket(GameServerSession::GetInstance(), pkt);
				//DBClientPacketHandler::HandlePacket()
				// ó��
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

	_cv.notify_one(); // ��� ���� ������ ����
}

void PacketWorker::NotifyReady()
{
	{
		std::unique_lock<std::mutex> lock(_cvMutex);
		_isReady = true;
	}

	_cv.notify_one();
}
