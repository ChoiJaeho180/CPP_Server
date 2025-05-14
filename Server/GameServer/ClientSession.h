#pragma once
#include "Session.h"

class ClientSession : public PacketSession {
public:
	ClientSession();
	~ClientSession();

	virtual void OnConnected() override;
	virtual void OnDisconnected() override;


	virtual void OnRecvPacket(BYTE* buffer, int32 len) override;
	virtual void OnSend(int32 len) override;

public:
	void AddPlayer(PlayerRef player) { _players.push_back(player); }
	PlayerRef GetPlayer(uint64 playerIndex) { return _players[playerIndex]; }
private:
	Vector<PlayerRef> _players;
};