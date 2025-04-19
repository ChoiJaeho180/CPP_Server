#include "pch.h"
#include "GameSession.h"
#include "GameSessionManager.h"

GameSession::GameSession() {

}
 
GameSession::~GameSession() {
	cout << "~GameSession" << endl;
}

void GameSession::OnConnected()
{
	GSessionManager.Add(static_pointer_cast<GameSession>(shared_from_this()));
}

void GameSession::OnDisconnected()
{
	GSessionManager.Remove(static_pointer_cast<GameSession>(shared_from_this()));
}

int32 GameSession::OnRecvPacket(BYTE* buffer, int32 len) {
	PacketHeader header = *(reinterpret_cast<PacketHeader*>(&buffer[0]));
	cout << "Pakcet Id : " << header.id << "Size : " << header.size << endl;

	return len;
}


void GameSession::OnSend(int32 len) {

}