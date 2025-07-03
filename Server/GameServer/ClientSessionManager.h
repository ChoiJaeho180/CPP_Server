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
	ClientSessionRef Get(const uint64 id);

private:
	USE_LOCK;
	HashMap<uint64, ClientSessionRef> _sessions;
};
