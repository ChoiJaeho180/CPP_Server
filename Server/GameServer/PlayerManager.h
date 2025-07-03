#pragma once

class Player;

class PlayerManager
{
public:
	static PlayerManager& GetInstance() {
		static PlayerManager instance;
		return instance;
	}

	void AddPlayer(PlayerRef player);

	PlayerRef GetPlayer(uint64 playerId);
	PlayerRef GetPlayer(std::string name);

	inline bool IsPlayerOnline(uint64 playerId) { return GetPlayer(playerId) != nullptr; }
	inline bool IsPlayerOnline(std::string name) { return GetPlayer(name) != nullptr; }
private:
	USE_LOCK;
	HashMap<uint64, PlayerRef> _idToPlayers;
	HashMap<std::string, PlayerRef> _nameToPlayers;

};

