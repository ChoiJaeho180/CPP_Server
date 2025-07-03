#include "pch.h"
#include "GameServerSession.h"
#include "DBClientPacketHandler.h"
#include "PacketShardManager.h"


GameServerSessionRef GameServerSession::_instance = nullptr;

void GameServerSession::OnConnected() {
	GameServerSession::SetInstance(static_pointer_cast<GameServerSession>(shared_from_this()));

	for (auto& worker : PacketShardManager::GetInstance().GetAllWorkers()) {
		worker.second->NotifyReady();
	}
}

void GameServerSession::OnDisconnected() {
	_instance = nullptr;
}

void GameServerSession::OnRecvPacket(BYTE* buffer, int32 len) {
	
	PacketSessionRef session = GetPacketSessionRef();
	std::optional<DBServerPacketRef> packet = DBClientPacketHandler::ParsePacket(buffer, len);
	if (packet.has_value() == false) {
		return;
	}
	
	PacketShardManager::GetInstance().EnqueuePacket(packet.value());
}

void GameServerSession::OnSend(int32 len) {

}
