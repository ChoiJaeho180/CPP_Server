#include "pch.h"
#include "PacketWorker.h"
#include "DBClientPacketHandler.h"
#include "GameServerSession.h"

PacketWorker::PacketWorker()
{
	LDbConnection = GDBConnectionPool->Pop();
}

PacketWorker::~PacketWorker()
{
	GDBConnectionPool->Push(LDbConnection);
	LDbConnection = nullptr;
}

void PacketWorker::Run()
{
	{
		// 1. Session�� ����� �� ���� ���.
		std::unique_lock<std::mutex> lock(_cvMutex);
		_cv.wait(lock, [&]() { return _isReady && GameServerSession::GetInstance(); });
	}

	while (true) {

		{
			// 2. ó���� ��Ŷ�� ������ ������ ���
			std::unique_lock<std::mutex> lock(_cvMutex);
			_cv.wait(lock, [&]() {
					READ_LOCK;
					return !_packets.empty();
				});
		}

		Queue<DBServerPacketRef> copyData;
		{
			// 3. ���� ó�� ���� ��Ŷ ť�� ����
			// �ּ����� �ð��� exclusive ���� ��� ���� (�� ���� �ּ�ȭ)
			// ���� ����� �����͸����� ó���ϹǷ� ó�� �߿��� Enqueue ����
			WRITE_LOCK;

			if (_packets.empty())
				continue;

			std::swap(copyData, _packets);
		}

		// 4. ���纻 ��Ŷ ó�� (�� ����)
		while (copyData.empty() == false) {
			DBServerPacketRef pkt = copyData.front();
			copyData.pop();

			cout << "Thread Id : " << LThreadId << ", pkt Id : " << pkt->header.id << endl;
			DBClientPacketHandler::HandlePacket(GameServerSession::GetInstance(), pkt);
		}
	}
}

void PacketWorker::EnqueuePacket(DBServerPacketRef pkt)
{
	{
		WRITE_LOCK;
		_packets.push(pkt);
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
