#include "pch.h"
#include "Room.h"
#include "Player.h"
#include "ObjectUtils.h"
#include "../ServerCore/SendBuffer.h"
#include "ClientPacketHandler.h"
#include "ClientSession.h"

Room::Room()
{
}

Room::~Room()
{
}

bool Room::ProcessEnterLocked(PlayerRef player)
{
	WRITE_LOCK;

	bool success = Enter(player);
	if (!success) {
		return false;
	}

	{
		Protocol::LocationYaw* locationYaw = new Protocol::LocationYaw();
		locationYaw->set_x(MathUtils::rand(0.f, 500.f));
		locationYaw->set_y(MathUtils::rand(0.f, 500.f));
		locationYaw->set_z(100.f);
		locationYaw->set_yaw(MathUtils::rand(0.f, 100.f));

		Protocol::PlayerInfo& curPlayerInfo = player->GetPlayerInfo();
		curPlayerInfo.set_allocated_locationyaw(locationYaw);

		Protocol::S_ENTER_GAME enterGamePkt;
		enterGamePkt.set_success(true);

		Protocol::PlayerInfo* playerInfo = new Protocol::PlayerInfo();
		playerInfo->CopyFrom(curPlayerInfo);
		enterGamePkt.set_allocated_player(playerInfo);

		if (auto session = player->OwnerSession().lock()) {
			SEND_PACKET(enterGamePkt);
		}
	}
	
	{
		Protocol::S_SPAWN spawnPkt;
		for (auto& item : _players) {
			if (item.first == player->GetPlayerInfo().id()) {
				continue;
			}

			Protocol::PlayerInfo* playerInfo = spawnPkt.add_players();
			playerInfo->CopyFrom(item.second->GetPlayerInfo());
		}

		if (auto session = player->OwnerSession().lock()) {
			SEND_PACKET(spawnPkt);
		}
	}

	{
		Protocol::S_SPAWN spawnPkt;

		Protocol::PlayerInfo* playerInfo = spawnPkt.add_players();
		playerInfo->CopyFrom(player->GetPlayerInfo());
		SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(spawnPkt);
		Broadcast(sendBuffer, player->GetPlayerInfo().id());
	}
	return true;
}


bool Room::ProcessLeaveLocked(PlayerRef player)
{
	WRITE_LOCK;

	const uint64 playerId = player->GetPlayerInfo().id();
	bool success = Leave(playerId);
	if (!success) {
		return false;
	}

	{
		Protocol::S_LEAVE_GAME leavGamePkt;

		if (auto session = player->OwnerSession().lock()) {
			SEND_PACKET(leavGamePkt);
		}
	}

	{
		Protocol::S_DESPAWN despawnPkt;
		despawnPkt.add_ids(playerId);
		SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(despawnPkt);
		Broadcast(sendBuffer, playerId);
	}

	return true;
}

void Room::ProcessMovePlayerLocked(Protocol::C_MOVE& pkt)
{
	WRITE_LOCK;

	const uint64 id = pkt.player().id();
	if (_players.find(id) == _players.end()) {
		return;
	}

	// todo. 스피드 핵 검사


	PlayerRef& player = _players[id];
	player->GetPlayerInfo().CopyFrom(pkt.player());

	{
		Protocol::S_MOVE movePkt;
		{
			Protocol::PlayerInfo* info = movePkt.mutable_player();
			info->CopyFrom(pkt.player());
		}

		SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(movePkt);
		Broadcast(sendBuffer, id);
	}

}
bool Room::Enter(PlayerRef player)
{
	if (_players.find(player->GetPlayerInfo().id()) != _players.end()) {
		std::cout << "duplicate entered player" << std::endl;
		return false;
	}

	_players.insert(make_pair(player->GetPlayerInfo().id(), player));
	return true;
}

bool Room::Leave(uint64 playerId)
{
	if (_players.find(playerId) == _players.end()) {
		std::cout << "not found user" << std::endl;
		return false;
	}

	_players.erase(playerId);
	return true;
}

bool Room::movePlayer(PlayerRef player)
{
	return false;
}

void Room::Broadcast(SendBufferRef sendBuffer, uint64 expectedId)
{
	for (auto& player : _players) {
		if (player.first == expectedId) {
			continue;
		}
		if (auto session = player.second->OwnerSession().lock()) {
			session->Send(sendBuffer);
		}
	}

}

