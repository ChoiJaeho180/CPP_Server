#pragma once
#include "Protocol.pb.h"

class Room
{
public:
	void				EnterRoom(PlayerRef player);
	void				LeaveRoom(PlayerRef player);
	void				BroadCast(SendBufferRef sendBuffer);
private:
	USE_LOCK;
	map<uint64, PlayerRef>		_players;
};

extern Room GRoom;