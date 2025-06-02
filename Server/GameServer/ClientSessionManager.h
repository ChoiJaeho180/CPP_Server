#pragma once

class ClientSession;

class ClientSessionManager
{
public:
	static ClientSessionManager& GetInstance() {
		static ClientSessionManager instance;
		return instance;
	}
public:
	void Add(ClientSessionRef session);
	void Remove(ClientSessionRef session);
	void Broadcast(SendBufferRef sendBuffer);
private:
	USE_LOCK;
	Set<ClientSessionRef> _sessions;
};
