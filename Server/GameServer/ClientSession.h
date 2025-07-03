#pragma once
#include "Session.h"

class ClientSession : public PacketSession {
public:
	ClientSession();
	~ClientSession();

	virtual void								OnConnected() override;
	virtual void								OnDisconnected() override;


	virtual void								OnRecvPacket(BYTE* buffer, int32 len) override;
	virtual void								OnSend(int32 len) override;

public:
	void										AddLobbyPlayer(Protocol::LobbyPlayerInfo lobbyPlayer) { _lobbyPlayersInfo[lobbyPlayer.id()] = lobbyPlayer; }
	bool										HasLobbyPlayer(uint64 playerId) { return _lobbyPlayersInfo.find(playerId) != _lobbyPlayersInfo.end(); }

	void										SetCurPlayer(PlayerRef player) { _curPlayer = player; }
	PlayerRef									GetCurPlayer() { return _curPlayer; }

private:
	Atomic<PlayerRef>							_curPlayer;
	HashMap<uint64, Protocol::LobbyPlayerInfo>	_lobbyPlayersInfo;
};