#pragma once

class Protocol::PlayerInfo;
class Room;
class ClientSession;

class Player : public enable_shared_from_this<Player>
{
public:
	Player() {}
	Player(string name, Protocol::PlayerType type, ClientSessionRef owner);
	~Player();
	string						Name() { return _name; }
	Protocol::PlayerType		PlayerType() { return _type; }
	weak_ptr<ClientSession>		OwnerSession() { return _ownerSession; }
public:

	Protocol::PlayerInfo&		GetPlayerInfo() { return _playerInfo; }
public:
	/* 처음 한번만 할당*/
	void						SetOwnerSession(shared_ptr<ClientSession> session) { _ownerSession = session; }
private:
	
	string							_name;
	Protocol::PlayerType			_type = Protocol::PlayerType::PLAYER_TYPE_NONE;
	weak_ptr<ClientSession>			_ownerSession; 
	Protocol::PlayerInfo			_playerInfo;
};

