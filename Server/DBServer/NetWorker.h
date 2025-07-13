#pragma once
#include "../ServerCore/pch.h"

class NetWorker
{
public:
	/* NetWorker�� ����ϴ� �Լ�*/
	void										Run(const ServerServiceRef& service);
	void										FlushSendQueue(const ServerServiceRef& service);
public:
	/* PacketWorker�� ����ϴ� �Լ�*/
	void										EnqueuePacket(SendBufferRef pkt);
private:
	USE_LOCK;
	Queue<SendBufferRef>						_packets;
};

using NetWorkerRef = shared_ptr<class NetWorker>;