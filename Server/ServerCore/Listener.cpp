#include "pch.h"
#include "Listener.h"
#include "SocketUtils.h"
#include "IocpEvent.h"

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
}

void Listener::RegisterAccept(AcceptEvent* acceptEvent)
{
	SocketUtils::AcceptEx(_socket,)
}

void Listener::ProcessAccept(AcceptEvent* acceptEvent)
{
}
