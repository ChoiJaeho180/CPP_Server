#include "pch.h"
#include "PlayerManager.h"
#include "Player.h"

PlayerRef PlayerManager::GetPlayer(uint64 playerId)
{
	READ_LOCK;
	auto it = _idToPlayers.find(playerId);
	if (it == _idToPlayers.end())
		return nullptr;

	return it->second;
}

PlayerRef PlayerManager::GetPlayer(std::string name)
{
	READ_LOCK;
	auto it = _nameToPlayers.find(name);
	if (it == _nameToPlayers.end())
		return nullptr;

	return it->second;
}

void PlayerManager::AddPlayer(PlayerRef player)
{
	WRITE_LOCK;
}