#include "pch.h"
#include "Player.h"

Player::Player(ClientSessionRef owner)
	: _ownerSession(owner)
{
}

Player::~Player() {
	cout << "~Player" << endl;
}

void Player::Update(float deltaTime)
{
}
