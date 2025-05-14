#include "pch.h"
#include "Player.h"

Player::Player(uint64 playerId, string name, Protocol::PlayerType type, ClientSessionRef owner)
	: _playerId(playerId), _name(name), _type(type), _ownerSession(owner)
{
}

Player::~Player() {
	cout << "~Player" << endl;
}