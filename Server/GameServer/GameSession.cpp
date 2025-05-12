#include "pch.h"
#include "GameSession.h"
#include "GameSessionManager.h"
#include "ClientPacketHandler.h"

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

void GameSession::OnRecvPacket(BYTE* buffer, int32 len) {
	PacketSessionRef sessionRef = GetPacketSessionRef();
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
	// todo. packetId 대역 체크
	ClientPacketHandler::HandlePacket(sessionRef, buffer, len);
}


void GameSession::OnSend(int32 len) {

}