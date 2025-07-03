#pragma once
#include "../ServerCore/CallbackStorage.h"
#include "DBServerPacketHandler.h"

class CallbackStorage;

class DBResponseManager
{
public:
	DBResponseManager();
	~DBResponseManager();

	CallbackContext							Take(const uint64 targetId, const uint64 requestId);
	uint16									GetShardId(const uint64 id);
	void									EnqueuePacket(DBPacketRef packet);
	void									ProcessPackets();
public:

	template<typename T>
	uint64 Add(uint64 id, std::function<void(const T&)> callback) {
		return _pendingCallbacks[GetShardId(id)]->Add(id, [=](void* ptr) {
				callback(*static_cast<T*>(ptr));
			});
	}
private:
	Vector<shared_ptr<CallbackStorage>>		_pendingCallbacks;

private:
	USE_LOCK;
	Queue<DBPacketRef>						_readyCallbacks;
};

