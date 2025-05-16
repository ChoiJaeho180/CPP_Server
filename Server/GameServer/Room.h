#pragma once
#include "TaskQueue.h"

class Room : public TaskQueue
{
public:
	// �̱� ������ ȯ��ó�� ����
	void				Enter(PlayerRef player);
	void				Leave(PlayerRef player);
	void				BroadCast(SendBufferRef sendBuffer);
public:

	

private:
	map<uint64, PlayerRef>		_players;
};

extern shared_ptr<Room> GRoom;
