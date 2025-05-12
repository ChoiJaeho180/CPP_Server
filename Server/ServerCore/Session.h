#pragma once

#include "IocpCore.h"
#include "IocpEvent.h"
#include "NetAddress.h"
#include "ReceiveBuffer.h"

class Service;
class Listener;
class Session : public IocpObject
{
	friend class Listener;
	friend class IocpCore;
	friend class Service;

	enum {
		BUFFER_SIZE = 0X10000 // 64KB
	};
public:
	Session();
	virtual ~Session();
public:
	/*외부에서 사용*/
	void					Send(SendBufferRef sendBuffer);
	bool					Connect();
	void					Disconnect(const WCHAR* cause);

	shared_ptr<Service>		GetService() { return _service.lock(); }
	void					SetService(shared_ptr<Service>service) { _service = service; }
public:
	inline void				SetNetAddress(NetAddress address) { _netAddress = address; }
	inline NetAddress		GetAddress() { return _netAddress; }
	inline SOCKET			GetSocket() { return _socket; }
	bool					IsConnected() { return _connected; }
	SessionRef				GetSessionRef() { return static_pointer_cast<Session>(shared_from_this()); }
public:
	/*인터페이스 구현*/
	virtual HANDLE			GetHandle() override;
	virtual void			Dispatch(class IocpEvent* iocpEvent, int32 numOfBytes = 0) override;

private:
	/*전송 관련*/
	bool					RegisterConnect();
	bool					RegisterDisconnect();
	void					RegisterRecv();
	void					RegisterSend();

	void					ProcessConnect();
	void					ProcessDisconnect();
	void					ProcessRecv(int32 numOfBytes);
	void					ProcessSend(int32 numOfBytes);
	void					HandleError(int32 errorCode);
protected:
	/* 컨텐츠 코드에서 재정의*/
	virtual void			OnConnected() {}
	virtual int32			OnRecv(BYTE* buffer, int32 len) { return len; }
	virtual void			OnSend(int32 len) {}
	virtual void			OnDisconnected(){}

private:
	weak_ptr<Service>		_service;
	SOCKET					_socket = INVALID_SOCKET;
	NetAddress				_netAddress;
	Atomic<bool>			_connected = false;
	
private:
	USE_LOCK;
	/*수신 관련*/
	ReceiveBuffer			_recvBuffer;

	/*송신 관련*/
	Queue<SendBufferRef>	_sendQueue;
	// lock free 구조로 변경될 떄 필요해서 일단 atomic으로
	// 지금은 필요없음
	Atomic<bool>			_sendRegistered = false;
private:
	/*IocpEvent 재사용*/
	ConnectEvent			_connectEvent;
	DisconnectEvent			_disconnectEvent;
	RecvEvent				_recvEvent;
	SendEvent				_sendEvent;
};

// PacketSession : 
// [size(2)][id(2)][data...][size(2)][id(2)][data...][size(2)][id(2)][data...]
struct PacketHeader {
	uint16 size;
	uint16 id; // proto id
};

class PacketSession : public Session {
public:
	PacketSession();
	virtual ~PacketSession();

	PacketSessionRef	GetPacketSessionRef() { return static_pointer_cast<PacketSession>(shared_from_this()); }

protected:
	virtual int32		OnRecv(BYTE* buffer, int32 len) sealed;
	virtual void		OnRecvPacket(BYTE* buffer, int32 len) abstract;
};