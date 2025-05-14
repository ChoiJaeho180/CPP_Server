#pragma once
#include "Task.h"

class Room
{
	friend class EnterTask;
	friend class LeaveTask;
	friend class BroadCastTask;

private:
	// �̱� ������ ȯ��ó�� ����
	void				Enter(PlayerRef player);
	void				Leave(PlayerRef player);
	void				BroadCast(SendBufferRef sendBuffer);
public:
	// ��Ƽ������ ȯ�濡���� �ϰ����� ����
	void PushTask(TaskRef task) { _tasks.Push(task); }
	void FlushTask();
private:
	map<uint64, PlayerRef>		_players;

	TaskQueue					_tasks;
};

extern Room GRoom;

class EnterTask : public ITask 
{
public:
	EnterTask(Room& room, PlayerRef player)
		: _room(room), _player(player)
	{

	}

	virtual void Execute() override {
		_room.Enter(_player);
	}

public:
	Room& _room;
	PlayerRef _player;
};

class LeaveTask : public ITask
{
public:
	LeaveTask(Room& room, PlayerRef player)
		: _room(room), _player(player)
	{

	}

	virtual void Execute() override {
		_room.Leave(_player);
	}
public:
	Room& _room;
	PlayerRef _player;
};

class BroadCastTask : public ITask
{
public:
	BroadCastTask(Room& room, SendBufferRef sendBuffer)
		: _room(room), _sendBuffer(sendBuffer)
	{

	}

	virtual void Execute() override {
		_room.BroadCast(_sendBuffer);
	}
public:
	Room& _room;
	SendBufferRef _sendBuffer;
};