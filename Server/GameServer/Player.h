#pragma once
class Player
{
public:
	Player() {}
	Player(uint64 playerId, string name, Protocol::PlayerType type, ClientSessionRef owner);
	~Player();
	uint64						PlayerId() { return _playerId; }
	string						Name() { return _name; }
	Protocol::PlayerType		PlayerType() { return _type; }
	weak_ptr<ClientSession>		ownerSession() { return _ownerSession; }
private:
	
	uint64							_playerId = 0;
	string							_name;
	Protocol::PlayerType			_type = Protocol::PlayerType::PLAYER_TYPE_NONE;
	weak_ptr<ClientSession>			_ownerSession; 
};

