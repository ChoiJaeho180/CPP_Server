#pragma once

// ����, NPC, AI, FSM ���� ���� ������ ������ ��
class ZoneInstance : public TaskQueue
{
public:
	ZoneInstance();
	virtual ~ZoneInstance();
public:
	void Update();
};

