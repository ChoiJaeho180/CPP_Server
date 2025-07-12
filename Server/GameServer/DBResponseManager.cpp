#include "pch.h"
#include "DBResponseManager.h"
#include "../ServerCore/CallbackStorage.h"
#include "ZoneManager.h"
#include "ZoneInstance.h"
#include "PlayerManager.h"
#include "Player.h"
#include "CallbackStorage.h"

DBResponseManager::DBResponseManager()
{
	_pendingCallbacks.reserve(GameConst::GAME_WORKER_COUNT);
	for (int i = 0; i < GameConst::GAME_WORKER_COUNT; i++) {
		_pendingCallbacks.push_back(MakeShared<CallbackStorage>());
	}

	_completedPackets.reserve(GameConst::GAME_WORKER_COUNT);
	for (int i = 0; i < GameConst::GAME_WORKER_COUNT; i++) {
		_completedPackets.push_back(Queue<DBPacketRef>());
	}
}

DBResponseManager::~DBResponseManager()
{

}

uint16 DBResponseManager::GetShardId(const uint64 id)
{
	return static_cast<uint16>(id % GameConst::GAME_WORKER_COUNT);
}

void DBResponseManager::EnqueuePacket(DBPacketRef packet)
{
	const int shardId = GetShardId(packet->header.targetId);

	WRITE_LOCK;
	_completedPackets[shardId].push(packet);
}

void DBResponseManager::ProcessPackets(const int& gameShardId)
{
	// todo. 멀티 스레드 환경에서 병목을 더 해소할 수 있는지 확인 필요..
	auto PopBatch = [&](int32 count, OUT Vector<DBPacketRef>& out)
		{
			WRITE_LOCK_IDX(gameShardId); // shard별 락 사용
			Queue<DBPacketRef>& queue = _completedPackets[gameShardId];
			for (int32 i = 0; i < count && !queue.empty(); ++i)
			{
				out.push_back(std::move(queue.front()));
				queue.pop();
			}
		};

	while (LEndTickCount > ::GetTickCount64()) {
		// todo. 처리 속도에 따라 10이 아니라 변동되도록
		Vector<DBPacketRef> packets;
		PopBatch(10, OUT packets);

		if (packets.empty()) {
			break;
		}

		// 1. 샤드별로 requestId 분리
		//   requestId는 각 CallbackStorage 내부에서만 고유함
		Vector<Vector<uint64>> shardToRequestIds(GameConst::GAME_WORKER_COUNT);
		for (const auto& pkt : packets)
		{
			uint64 targetId = pkt->header.targetId;
			uint16 shardId = GetShardId(targetId);
			
			shardToRequestIds[shardId].push_back(pkt->header.requestId);
		}

		// 2. 모든 콜백 모아오기
		Vector<HashMap<uint64, CallbackContextRef>> shardedCallbacks(GameConst::GAME_WORKER_COUNT);
		for (int shardId = 0; shardId < shardToRequestIds.size(); shardId++) {
			if (shardToRequestIds[shardId].empty()) {
				continue;
			}

			_pendingCallbacks[shardId]->TakeBatch(shardToRequestIds[shardId], OUT shardedCallbacks[shardId]);
		}
			
		// 3. TaskQueue에 Push 및 Execute
		for (int i = 0; i < packets.size(); i++) {
			const DBPacketRef& pkt = packets[i];

			const uint64 id = pkt->header.targetId;
			const uint16 shardId = GetShardId(id);
			const uint64 requestId = pkt->header.requestId;

			auto it = shardedCallbacks[shardId].find(requestId);
			ASSERT_CRASH(it != shardedCallbacks[shardId].end());
			CallbackContextRef ctx = it->second;

			PlayerRef player = PlayerManager::GetInstance().GetPlayer(id);
			if (player) {
				auto zoneInstnace = player->OwnerZoneInstance().lock();
				if (!zoneInstnace) {
					continue;
				}

				TaskRef task = ObjectPool<Task>::MakeShared([ctx, pkt]() {
					ExecuteCallback(ctx, pkt);
				});

				zoneInstnace->Push(task);
			}
			else {
				// zone에 입장하지 않는 경우 
				// ex) login, enterWorld 처리 중이나 그 전인 경우
				ExecuteCallback(ctx, pkt);
			}
		}
	}
}

 void DBResponseManager::ExecuteCallback(CallbackContextRef ctx, const DBPacketRef& pkt)
{
	try {
		if (ctx->onComplete) {
			ctx->onComplete(pkt->data.get());
		}
	}
	catch (const std::exception& e) {
		cout << "DB callback exception: " << e.what() << endl;
	}
}
