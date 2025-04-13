#pragma once

class Session;
enum class EventType : uint8
{
	Connect,
	Disconnect,
	Accept,
	//PreRecv,
	Recv,
	Send
};

// 주의 : virtual 키워드 사용하면 안됨
// 이유 : 가상함수 테이블이 offset 0번 메모리에 들어가게 되면서
//        OVERLAPPED 주소가 아니라 다른걸로 채워질 수 있음.
class IocpEvent : public OVERLAPPED
{
public:
	IocpEvent(EventType type);

	void Init();
	
public:
	EventType eventType;
	IocpObjectRef owner;
};

class ConnectEvent : public IocpEvent
{
public:
	ConnectEvent():IocpEvent(EventType::Connect){}
};

class DisconnectEvent : public IocpEvent
{
public:
	DisconnectEvent() :IocpEvent(EventType::Disconnect) {}
};

class AcceptEvent : public IocpEvent
{
public:
	AcceptEvent() :IocpEvent(EventType::Accept) {}

public:
	SessionRef session;
	
};

class RecvEvent : public IocpEvent
{
public:
	RecvEvent() :IocpEvent(EventType::Recv) {}
};

class SendEvent : public IocpEvent
{
public:
	SendEvent() :IocpEvent(EventType::Send) {}

	Vector<SendBufferRef> sendBuffers;
};
