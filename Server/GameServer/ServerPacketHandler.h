#pragma once
#include "BufferReader.h"
#include "BufferWriter.h"
#include "Protocol.pb.h"

enum {
	S_TEST = 1
};

class ServerPacketHandler
{
public:
	static void HandlePacket(BYTE* buffer, int32 len);
	static SendBufferRef MakeSendBuffer(Protocol::S_TEST& pkt);
};

template<typename T>
SendBufferRef _MakeSendBuffer(T& pkt, uint16 pktId) {
	const uint16 dataSize = static_cast<uint16>(pkt.ByteSizeLong());
	const uint16 packetSize = dataSize + sizeof(PacketHeader);

	SendBufferRef sendBufferRef = GSendBufferManager->Open(packetSize);

	PacketHeader* header = reinterpret_cast<PacketHeader*>(sendBufferRef->Buffer());
	header->id = pktId;
	header->size = packetSize;

	ASSERT_CRASH(pkt.SerializeToArray(&header[1], dataSize));

	sendBufferRef->Close(packetSize);
	return sendBufferRef;
}
