#include "pch.h"
#include "ClientSession.h"
#include "ClientSessionManager.h"
#include "ClientPacketHandler.h"
#include "Room.h"
#include "Player.h"
#include <algorithm>

ClientSession::ClientSession() {

}

ClientSession::~ClientSession() {
	cout << "~ClientSession" << endl;
}

void ClientSession::OnConnected()
{
	GSessionManager.Add(static_pointer_cast<ClientSession>(shared_from_this()));
}

void ClientSession::OnDisconnected()
{
	GSessionManager.Remove(static_pointer_cast<ClientSession>(shared_from_this()));
	GRoom.LeaveRoom(GetPlayer(0));
	for (auto& player : _players) {
		player = nullptr;
	}
}

void ClientSession::OnRecvPacket(BYTE* buffer, int32 len) {
	PacketSessionRef session = GetPacketSessionRef();
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
	// todo. packetId 대역 체크
	ClientPacketHandler::HandlePacket(session, buffer, len);
}


void ClientSession::OnSend(int32 len) {

}

