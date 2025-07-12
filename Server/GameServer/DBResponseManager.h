#pragma once
#include "../ServerCore/CallbackStorage.h"
#include "DBServerPacketHandler.h"

class CallbackStorage;

class DBResponseManager
{
public:
	DBResponseManager();
	~DBResponseManager();

	
	uint16									GetShardId(const uint64 id);
public:
	/* DbWorkerThread  전용 함수*/
	void									EnqueuePacket(DBPacketRef packet);

public:
	/* GameWorkerThread 전용 함수*/
	void									ProcessPackets(const int& gameShardId);
	static void								ExecuteCallback(CallbackContextRef ctx, const DBPacketRef& pkt);

	template<typename T>
	uint64 Add(uint64 id, std::function<void(const T&)> callback) {
		return _pendingCallbacks[GetShardId(id)]->Add(id, [=](void* ptr) {
			callback(*static_cast<T*>(ptr));
			});
	}
	
private:
	// 샤드 playerId % GameConst::DB_WORKER_COUNT
	Vector<shared_ptr<CallbackStorage>>		_pendingCallbacks;

private:
	// 샤드 playerId % GameConst::GAME_WORKER_COUNT
	USE_MANY_LOCKS(GameConst::GAME_WORKER_COUNT);
	Vector<Queue<DBPacketRef>>				_completedPackets;
};

