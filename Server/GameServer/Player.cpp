#include "pch.h"
#include "Player.h"

Player::Player(string name, Protocol::PlayerType type, ClientSessionRef owner)
	: _name(name), _type(type), _ownerSession(owner)
{
}

Player::~Player() {
	cout << "~Player" << endl;
}

void Player::Update(float deltaTime)
{
}
