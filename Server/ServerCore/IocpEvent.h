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

// ���� : virtual Ű���� ����ϸ� �ȵ�
// ���� : �����Լ� ���̺��� offset 0�� �޸𸮿� ���� �Ǹ鼭
//        OVERLAPPED �ּҰ� �ƴ϶� �ٸ��ɷ� ä���� �� ����.
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
