#pragma once
#include "../ServerCore/TaskQueue.h"
#include <unordered_map>

class Room : public TaskQueue
{
public:
	Room();
	~Room();

public:
	static Room& GetInstance() {
		static Room instance;
		return instance;
	}

	bool ProcessEnterLocked(PlayerRef player);
	bool Enter(PlayerRef player);
	void Leave(uint64 playerId);
private:

	void Broadcast(SendBufferRef sendBuffer, uint64 expectedId = 0);
private:
	USE_LOCK;
	unordered_map<uint64, PlayerRef> _players;
};

