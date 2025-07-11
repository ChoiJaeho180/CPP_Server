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

bool Room::ProcessEnter(PlayerRef player)
{
	bool success = Enter(player);
	if (!success) {
		return false;
	}

	Protocol::ObjectInfo& enterPlayerInfo = player->GetObjectInfo();
	{
		Protocol::LocationYaw* locationYaw = new Protocol::LocationYaw();
		locationYaw->set_x(MathUtils::rand(0.f, 500.f));
		locationYaw->set_y(MathUtils::rand(0.f, 500.f));
		locationYaw->set_yaw(MathUtils::rand(0.f, 100.f));
		enterPlayerInfo.mutable_locationyaw()->CopyFrom(*locationYaw);

		Protocol::S_ENTER_GAME enterGamePkt;
		enterGamePkt.set_success(true);

		Protocol::ObjectInfo* playerInfo = new Protocol::ObjectInfo();
		playerInfo->CopyFrom(enterPlayerInfo);
		enterGamePkt.set_allocated_player(playerInfo);

		if (auto session = player->OwnerSession().lock()) {
			SEND_PACKET(enterGamePkt);
		}
	}
	
	// 접속한 유저에게 이전에 접속해 있던 주변 유저 정보 전달.
	{
		Protocol::S_SPAWN spawnPkt;
		for (auto& item : _players) {
			if (item.first == enterPlayerInfo.id()) {
				continue;
			}

			Protocol::ObjectInfo* playerInfo = spawnPkt.add_objects();
			playerInfo->CopyFrom(item.second->GetObjectInfo());
		}

		if (auto session = player->OwnerSession().lock()) {
			SEND_PACKET(spawnPkt);
		}
	}

	// 주변 유저에게 현재 유저 정보 전달
	{
		Protocol::S_SPAWN spawnPkt;

		Protocol::ObjectInfo* playerInfo = spawnPkt.add_objects();
		playerInfo->CopyFrom(enterPlayerInfo);
		SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(spawnPkt);
		Broadcast(sendBuffer, enterPlayerInfo.id());
	}
	return true;
}


bool Room::ProcessLeave(PlayerRef player)
{
	const uint64 playerId = player->GetObjectInfo().id();
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

void Room::ProcessMove(Protocol::C_MOVE pkt)
{
	const Protocol::LocationYaw newLocationYaw = pkt.locationyaw();
	const uint64 id = newLocationYaw.id();
	if (_players.find(id) == _players.end()) {
		return;
	}

	// todo. 스피드 핵 검사

	PlayerRef& player = _players[id];
	Protocol::LocationYaw* locationYaw = player->GetObjectInfo().mutable_locationyaw();
	locationYaw->CopyFrom(newLocationYaw);

	{
		Protocol::S_MOVE movePkt;
		{
			Protocol::LocationYaw* sendLoc = movePkt.mutable_locationyaw();
			sendLoc->CopyFrom(newLocationYaw);
		/*	cout << "id : " << id << endl;
			cout << " locationYaw X : " << sendLoc->x() << ", Y : " << sendLoc->y() << ", Z : " << endl;*/
		}

		SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(movePkt);
		Broadcast(sendBuffer, id);
	}

}
bool Room::Enter(PlayerRef player)
{
	if (_players.find(player->GetObjectInfo().id()) != _players.end()) {
		std::cout << "duplicate entered player" << std::endl;
		return false;
	}

	_players.insert(make_pair(player->GetObjectInfo().id(), player));
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

bool Room::move(PlayerRef player)
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

