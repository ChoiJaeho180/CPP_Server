#pragma once

#include "IocpCore.h"
#include "IocpEvent.h"
#include "NetAddress.h"

class Session : public IocpObject
{
public:
	Session();
	virtual ~Session();
public:
	inline void SetNetAddress(NetAddress address) { _netAddress = address; }
	inline NetAddress GetAddress() { return _netAddress; }
	inline SOCKET GetSocket() { return _socket; }
public:
	virtual HANDLE GetHandle() override;
	virtual void Dispatch(class IocpEvent* iocpEvent, int32 numOfBytes = 0) override;
public:
	// temp
	char _recvBuffer[1000];

private:
	SOCKET _socket = INVALID_SOCKET;
	NetAddress _netAddress;
	Atomic<bool> _connected = false;

};

