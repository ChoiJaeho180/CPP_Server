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
	if (success == false) {
		return false;
	}

	{
		Protocol::LocationYaw* locationYaw = new Protocol::LocationYaw();
		locationYaw->set_x(MathUtils::rand(0.f, 500.f));
		locationYaw->set_y(MathUtils::rand(0.f, 500.f));
		locationYaw->set_z(MathUtils::rand(0.f, 500.f));
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

bool Room::Enter(PlayerRef player)
{
	if (_players.find(player->GetPlayerInfo().id()) != _players.end()) {
		std::cout << "duplicate entered player" << std::endl;
		return false;
	}
	
	_players.insert(make_pair(player->GetPlayerInfo().id(), player));
	return true;
}

void Room::Leave(uint64 playerId)
{
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

