#include "pch.h"
#include "Listener.h"
#include "SocketUtils.h"
#include "IocpEvent.h"
#include "Session.h"

Listener::~Listener()
{
	SocketUtils::Close(_socket);
	for (AcceptEvent* acceptEvent : _acceptEvents) {
		xdelete(acceptEvent);
	}
}

bool Listener::StartAccept(NetAddress netAddress)
{
	_socket = SocketUtils::CreateSocket();
	if (_socket == INVALID_SOCKET) {
		return false;
	}
	if (GIocpCore.Register(this) == false) {
		return false;
	}
	
	if (SocketUtils::SetReuseAddress(_socket, true) == false) {
		return false;
	}

	if (SocketUtils::SetLinger(_socket, 0, 0) == false) {
		return false;
	}

	if (SocketUtils::Bind(_socket, netAddress) == false) {
		return false;
	}

	if (SocketUtils::Listen(_socket) == false) {
		return false;
	}
	
	const int32 acceptCount = 1;
	for (int32 i = 0; i < acceptCount; i++) {
		AcceptEvent* acceptEvent = xnew<AcceptEvent>();
		_acceptEvents.push_back(acceptEvent);
		RegisterAccept(acceptEvent);
	}

	return true;
}

void Listener::CloseSocket()
{
	SocketUtils::Close(_socket);
}

HANDLE Listener::GetHandle()
{
	return reinterpret_cast<HANDLE>(_socket);
}

void Listener::Dispatch(IocpEvent* iocpEvent, int32 numOfBytes)
{
	ASSERT_CRASH(iocpEvent->GetType() == EventType::Accept);
	
	AcceptEvent* acceptEvent = static_cast<AcceptEvent*>(iocpEvent);
	ProcessAccept(acceptEvent);

}

void Listener::RegisterAccept(AcceptEvent* acceptEvent)
{
	Session* session = xnew<Session>();
	acceptEvent->Init();
	acceptEvent->SetSession(session);

	DWORD byteReceived = 0;
	if (false == SocketUtils::AcceptEx(_socket, session->GetSocket(), session->_recvBuffer, 0,
		sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16,
		OUT & byteReceived, static_cast<LPOVERLAPPED>(acceptEvent))) {
		const int32 err = ::WSAGetLastError();
		if (err != WSA_IO_PENDING) {
			// 
			RegisterAccept(acceptEvent);
		}
	}
}

void Listener::ProcessAccept(AcceptEvent* acceptEvent)
{
	Session* session = acceptEvent->GetSession();

	if (SocketUtils::SetUpdateAcceptSocket(session->GetSocket(), _socket) == false) {
		RegisterAccept(acceptEvent);
		return;
	}

	SOCKADDR_IN sockAddress;
	int32 sizeofSockAddress = sizeof(SOCKADDR_IN);
	if (SOCKET_ERROR == ::getpeername(session->GetSocket(), reinterpret_cast<SOCKADDR*>(&sockAddress), &sizeofSockAddress)) {
		RegisterAccept(acceptEvent);
		return;
	}

	session->SetNetAddress((NetAddress)sockAddress);

	cout << "Client Connect" << endl;



	RegisterAccept(acceptEvent);
}
