#pragma once

// 유저, NPC, AI, FSM 실제 게임 로직을 돌리는 곳
class ZoneInstance : public TaskQueue
{
public:
	ZoneInstance();
	virtual ~ZoneInstance();
public:
	void Update();
};

