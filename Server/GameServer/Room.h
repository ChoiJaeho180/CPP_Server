#pragma once
#include "TaskQueue.h"

class Room : public TaskQueue
{
public:
	// 싱글 스레드 환경처럼 개발
	void				Enter(PlayerRef player);
	void				Leave(PlayerRef player);
	void				BroadCast(SendBufferRef sendBuffer);
public:

	

private:
	map<uint64, PlayerRef>		_players;
};

extern shared_ptr<Room> GRoom;
