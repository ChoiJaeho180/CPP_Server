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
	/*�ܺο��� ���*/
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
	/*�������̽� ����*/
	virtual HANDLE			GetHandle() override;
	virtual void			Dispatch(class IocpEvent* iocpEvent, int32 numOfBytes = 0) override;

private:
	/*���� ����*/
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
	/* ������ �ڵ忡�� ������*/
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
	/*���� ����*/
	ReceiveBuffer			_recvBuffer;

	/*�۽� ����*/
	Queue<SendBufferRef>	_sendQueue;
	// lock free ������ ����� �� �ʿ��ؼ� �ϴ� atomic����
	// ������ �ʿ����
	Atomic<bool>			_sendRegistered = false;
private:
	/*IocpEvent ����*/
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