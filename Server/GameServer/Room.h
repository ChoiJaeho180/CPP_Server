#pragma once
#include "../ServerCore/TaskQueue.h"
#include <unordered_map>

class Room : public TaskQueue //, public enable_shared_from_this<Room>
{
public:
	Room();
	~Room();

public:
	static Room& GetInstance() {
		static Room instance;
		return instance;
	}

	bool ProcessEnter(PlayerRef player);
	bool ProcessLeave(PlayerRef player);
	void ProcessMove(Protocol::C_MOVE pkt);
private:

	bool Enter(PlayerRef player);
	bool Leave(uint64 playerId);
	bool move(PlayerRef player);
private:

	void Broadcast(SendBufferRef sendBuffer, uint64 expectedId = 0);
private:
	unordered_map<uint64, PlayerRef> _players;
};

