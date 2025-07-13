#pragma once
#include "Session.h"

using GameServerSessionRef = shared_ptr<class GameServerSession>;

class GameServerSession : public PacketSession
{
public:
	GameServerSession() {};
	~GameServerSession() {};

	static void SetInstance(GameServerSessionRef session) { _instance = session; }
	static GameServerSessionRef GetInstance() { return _instance; }


	virtual void				OnConnected() override;
	virtual void				OnDisconnected() override;

	virtual void				OnRecvPacket(BYTE* buffer, int32 len) override;
	virtual void				OnSend(int32 len) override;

private:
	static GameServerSessionRef _instance;
};

