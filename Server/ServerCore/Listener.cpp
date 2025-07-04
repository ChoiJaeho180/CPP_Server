#include "pch.h"
#include "Listener.h"
#include "SocketUtils.h"
#include "IocpEvent.h"
#include "Session.h"
#include "Service.h"

Listener::~Listener()
{
	SocketUtils::Close(_socket);
	for (AcceptEvent* acceptEvent : _acceptEvents) {
		xdelete(acceptEvent);
	}
}

bool Listener::StartAccept(ServerServiceRef service)
{
	_service = service;
	if (_service == nullptr) {
		return false;
	}

	_socket = SocketUtils::CreateSocket();
	if (_socket == INVALID_SOCKET) {
		return false;
	}
	if (_service->GetIocpCore()->Register(shared_from_this()) == false) {
		return false;
	}
	
	if (SocketUtils::SetReuseAddress(_socket, true) == false) {
		return false;
	}

	if (SocketUtils::SetLinger(_socket, 0, 0) == false) {
		return false;
	}

	if (SocketUtils::Bind(_socket, _service->GetNetAddress()) == false) {
		return false;
	}

	if (SocketUtils::Listen(_socket) == false) {
		return false;
	}
	

	// 서버가 시작할 때 RegisterAccept()를 호출해서, 클라이언트 연결을 받을 준비를 미리 해둬야함.
	const int32 acceptCount = _service->GetMaxSessionCount();
	for (int32 i = 0; i < acceptCount; i++) {
		AcceptEvent* acceptEvent = xnew<AcceptEvent>();
		acceptEvent->owner = shared_from_this(); 
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
	ASSERT_CRASH(iocpEvent->eventType == EventType::Accept);
	
	AcceptEvent* acceptEvent = static_cast<AcceptEvent*>(iocpEvent);
	ProcessAccept(acceptEvent);
}

void Listener::RegisterAccept(AcceptEvent* acceptEvent)
{
	SessionRef session = _service->CreateSession(); // 
	acceptEvent->Init();
	acceptEvent->session = session;

	DWORD byteReceived = 0;
	if (false == SocketUtils::AcceptEx(_socket, session->GetSocket(), session->_recvBuffer.WritePos(), 0,
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
	SessionRef session = acceptEvent->session;

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
	session->ProcessConnect();
	cout << "Client Connect..  SessionId : " << session->GetSessionId() << endl;
	

	RegisterAccept(acceptEvent);
}
