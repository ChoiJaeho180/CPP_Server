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
	bool ProcessLeaveLocked(PlayerRef player);
	void ProcessMovePlayerLocked(Protocol::C_MOVE& pkt);
private:

	bool Enter(PlayerRef player);
	bool Leave(uint64 playerId);
	bool movePlayer(PlayerRef player);
private:

	void Broadcast(SendBufferRef sendBuffer, uint64 expectedId = 0);
private:
	USE_LOCK;
	unordered_map<uint64, PlayerRef> _players;
};

