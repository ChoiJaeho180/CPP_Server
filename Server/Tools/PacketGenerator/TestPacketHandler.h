#pragma once
#include "Protocol.pb.h"

using PacketHandlerFunc = std::function<bool(PacketSessionRef&, BYTE*, int32)>;
extern PacketHandlerFunc GPacketHandler[UINT16_MAX];


enum : uint16 {
	PKT_C_TEST = 1000,
	PKT_C_MOVE = 1001,
	PKT_S_TEST = 1002,
	PKT_S_LOGIN = 1003,
};

// Custom Handlers
bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len);
bool Handle_C_TEST(PacketSessionRef& sessionRef, Protocol::C_TEST& pkt);
bool Handle_C_MOVE(PacketSessionRef& sessionRef, Protocol::C_MOVE& pkt);

class TestPacketHandler
{
public:

	static void Init() {
		for (int i = 0; i < UINT16_MAX; i++) {
			GPacketHandler[i] = Handle_INVALID;
		}
		GPacketHandler[PKT_C_TEST] = [](PacketSessionRef& sessionRef, BYTE* buffer, int32 len) {return HandlePacket<Protocol::C_TEST>(Handle_C_TEST, sessionRef, buffer, len); };
		GPacketHandler[PKT_C_MOVE] = [](PacketSessionRef& sessionRef, BYTE* buffer, int32 len) {return HandlePacket<Protocol::C_MOVE>(Handle_C_MOVE, sessionRef, buffer, len); };
	}

	static bool HandlePacket(PacketSessionRef& session, BYTE* buffer, int32 len) {
		PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
		return GPacketHandler[header->id](session, buffer, len);
	}
	static SendBufferRef MakeSendBuffer(Protocol::S_TEST& pkt) { return MakeSendBuffer(pkt, PKT_S_TEST); }
	static SendBufferRef MakeSendBuffer(Protocol::S_LOGIN& pkt) { return MakeSendBuffer(pkt, PKT_S_LOGIN); }

private:

	template<typename PacketType, typename ProcessFunc>
	static bool HandlePacket(ProcessFunc func, PacketSessionRef& session, BYTE* buffer, int32 len) {
		PacketType pkt;
		if (pkt.ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)) == false) {
			return false;
		}

		return func(session, pkt);
	}

	template<typename T>
	static SendBufferRef MakeSendBuffer(T& pkt, uint16 pktId) {
		const uint16 dataSize = static_cast<uint16>(pkt.ByteSizeLong());
		const uint16 packetSize = dataSize + sizeof(PacketHeader);

		SendBufferRef sendBuffer = GSendBufferManager->Open(packetSize);

		PacketHeader* header = reinterpret_cast<PacketHeader*>(sendBuffer->Buffer());
		header->id = pktId;
		header->size = packetSize;

		ASSERT_CRASH(pkt.SerializeToArray(&header[1], dataSize));

		sendBuffer->Close(packetSize);
		return sendBuffer;
	}

};

