#pragma once

class ZoneInstance;
class ClientSession;

class Player : public BaseEntity, public IUpdate, public enable_shared_from_this<Player>
{
public:
	Player() {}
	Player(ClientSessionRef owner);
	virtual ~Player();


public:
	virtual void					Update(float deltaTime) override;
public:
	weak_ptr<ClientSession>			OwnerSession() { return _ownerSession; }
	weak_ptr<ZoneInstance>			OwnerZoneInstance() { return _ownerZoneInstance; }
public:
	/* ó�� �ѹ��� �Ҵ�*/
	void							SetOwnerSession(shared_ptr<ClientSession> session) { _ownerSession = session; }
private:
	weak_ptr<ClientSession>			_ownerSession; 
	weak_ptr<ZoneInstance>			_ownerZoneInstance;
};

