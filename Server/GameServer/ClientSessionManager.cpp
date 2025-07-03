#include "pch.h"
#include "ClientSessionManager.h"
#include "ClientSession.h"


void ClientSessionManager::Add(ClientSessionRef session)
{
	WRITE_LOCK;
	_sessions[session->GetSessionId()] = session;
}

void ClientSessionManager::Remove(ClientSessionRef session)
{
	WRITE_LOCK; 
	_sessions.erase(session->GetSessionId());
}

void ClientSessionManager::Broadcast(SendBufferRef sendBuffer)
{
	WRITE_LOCK;

	for (auto& elem : _sessions) {
		elem.second->Send(sendBuffer);
	}
}

ClientSessionRef ClientSessionManager::Get(const uint64 id)
{
	READ_LOCK;
	auto it = _sessions.find(id);
	if (it != _sessions.end())
		return it->second;

	return nullptr;
}
