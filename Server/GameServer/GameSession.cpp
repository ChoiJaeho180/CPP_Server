#include "pch.h"
#include "GameSession.h"
#include "GameSessionManager.h"

GameSession::GameSession() {

}
 
GameSession::~GameSession() {
	cout << "~GameSession" << endl;
}

void GameSession::OnConnected()
{
	GSessionManager.Add(static_pointer_cast<GameSession>(shared_from_this()));
}

void GameSession::OnDisconnected()
{
	GSessionManager.Remove(static_pointer_cast<GameSession>(shared_from_this()));
}

int32 GameSession::OnRecv(BYTE* buffer, int32 len) {
	cout << "OnRecv Len = " << len << endl;

	SendBufferRef sendBuffRef = GSendBufferManager->Open(4096);
	::memcpy(sendBuffRef->Buffer(), buffer, len);
	sendBuffRef->Close(len);
	
	GSessionManager.Broadcast(sendBuffRef);
	

	return len;
}


void GameSession::OnSend(int32 len) {
	cout << "OnSend Len = " << len << endl;
}