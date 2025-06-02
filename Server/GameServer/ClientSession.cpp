#include "pch.h"
#include "ClientSession.h"
#include <algorithm>
#include "ClientPacketHandler.h"
#include "Player.h"
#include "ClientSessionManager.h"
ClientSession::ClientSession() {

}

ClientSession::~ClientSession() {
	cout << "~ClientSession" << endl;
}

void ClientSession::OnConnected()
{
	ClientSessionManager::GetInstance().Add(static_pointer_cast<ClientSession>(shared_from_this()));
}

void ClientSession::OnDisconnected()
{
	ClientSessionManager::GetInstance().Remove(static_pointer_cast<ClientSession>(shared_from_this()));

	_curPlayer = nullptr;
	_players.clear();
}

void ClientSession::OnRecvPacket(BYTE* buffer, int32 len) {
	PacketSessionRef session = GetPacketSessionRef();
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
	// todo. packetId 대역 체크
	ClientPacketHandler::HandlePacket(session, buffer, len);
}


void ClientSession::OnSend(int32 len) {

}

