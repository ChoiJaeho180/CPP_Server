#pragma once

class Room;
class ClientSession;

class Player : public BaseEntity, public IUpdate, public enable_shared_from_this<Player>
{
public:
	Player() {}
	Player(ClientSessionRef owner);
	virtual ~Player();

public:
	virtual void Update(float deltaTime) override;
public:
	weak_ptr<ClientSession>		OwnerSession() { return _ownerSession; }
public:
	/* 처음 한번만 할당*/
	void						SetOwnerSession(shared_ptr<ClientSession> session) { _ownerSession = session; }
private:
	weak_ptr<ClientSession>			_ownerSession; 
};

