#pragma once
#include "../ServerCore/pch.h"

class NetWorker
{
public:
	/* NetWorker가 사용하는 함수*/
	void										Run(const ServerServiceRef& service);
	void										FlushSendQueue(const ServerServiceRef& service);
public:
	/* PacketWorker가 사용하는 함수*/
	void										EnqueuePacket(SendBufferRef pkt);
private:
	USE_LOCK;
	Queue<SendBufferRef>						_packets;
};

using NetWorkerRef = shared_ptr<class NetWorker>;