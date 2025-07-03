#include "pch.h"
#include "DBResponseManager.h"
#include "../ServerCore/CallbackStorage.h"
#include "ZoneManager.h"
#include "ZoneInstance.h"
#include "PlayerManager.h"
#include "Player.h"

DBResponseManager::DBResponseManager()
{
	_pendingCallbacks.reserve(GameConst::DB_JOB_SHARD_COUNT);
	for (int i = 0; i < GameConst::DB_JOB_SHARD_COUNT; i++) {
		_pendingCallbacks.push_back(MakeShared<CallbackStorage>());
	}
}

DBResponseManager::~DBResponseManager()
{

}

CallbackContext DBResponseManager::Take(const uint64 targetId, const uint64 requestId)
{
	const uint16 shardId = GetShardId(targetId);
	cout << "DBResponseManager::Take targetId : " << targetId << endl;
	cout << "DBResponseManager::Take shardId : " << shardId << endl;
	return _pendingCallbacks[shardId]->Take(requestId);
}

uint16 DBResponseManager::GetShardId(const uint64 id) {
	return static_cast<uint16>(id % GameConst::DB_JOB_SHARD_COUNT);
}

void DBResponseManager::EnqueuePacket(DBPacketRef packet)
{
	WRITE_LOCK;
	_readyCallbacks.push(packet);
}

void DBResponseManager::ProcessPackets()
{

	// todo. 멀티 스레드 환경에서 병목을 더 해소할 수 있는지 확인 필요..

	auto PopBatch = [&](int32 count, OUT Vector<DBPacketRef>& out)
		{
			WRITE_LOCK;
			for (int32 i = 0; i < count && !_readyCallbacks.empty(); ++i)
			{
				out.push_back(std::move(_readyCallbacks.front()));
				_readyCallbacks.pop();
			}
		};

	while (LEndTickCount > ::GetTickCount64()) {
		Vector<DBPacketRef> local;
		PopBatch(10, OUT local);

		if (local.empty()) {
			break;
		}
		cout << "ProcessPackets ThreadId : " << LThreadId << ", local size : " << local.size() << endl;
		for (int i = 0; i < local.size(); i++) {
			const DBPacketRef& pkt = local[i];
			const uint64 id = pkt->header.targetId;


			TaskRef task = ObjectPool<Task>::MakeShared([pkt]() {
					auto ctx = GDBServerCallbackMgr->Take(pkt->header.targetId, pkt->header.requestId);
					if (ctx.onComplete) {
						ctx.onComplete(pkt->data.get());
					}
				});

			PlayerRef player = PlayerManager::GetInstance().GetPlayer(id);
			if (player) {
				if (auto zoneInstnace = player->OwnerZoneInstance().lock()) {
					zoneInstnace->Push(task);
				}
			}
			else {
				// zone에 입장하지 않는 경우 
				// ex) login, enterWorld 처리 중이나 그 전인 경우
				CallbackContext ctx = GDBServerCallbackMgr->Take(pkt->header.targetId, pkt->header.requestId);
				if (ctx.onComplete) {
					ctx.onComplete(pkt->data.get());
				}
			}
		}
	}
}
