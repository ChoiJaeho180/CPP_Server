#pragma once
#include "DBProtocol.pb.h"

using PacketFactoryFunc = std::function<std::shared_ptr<google::protobuf::Message>()>;
extern PacketFactoryFunc GPacketFactory[UINT16_MAX];

struct DBPacket {
	ServerPacketHeader header;
	std::shared_ptr<google::protobuf::Message> data;
};
using DBPacketRef = std::shared_ptr<DBPacket>;

enum : uint16 {
	PKT_C_LOAD_LOBBY_PLAYERS_INFO = 10000,
	PKT_S_LOAD_LOBBY_PLAYERS_INFO = 10001,
};

class DBServerPacketHandler
{
public:

	static void Init() {
		for (int i = 0; i < UINT16_MAX; i++) {
			GPacketFactory[i] = nullptr;
		}
		GPacketFactory[PKT_S_LOAD_LOBBY_PLAYERS_INFO] = []() { return ObjectPool<DBProtocol::S_LOAD_LOBBY_PLAYERS_INFO>::MakeShared(); };
	}

	static DBPacketRef ParsePacket(BYTE* buffer, int32 len)
	{
		ServerPacketHeader* header = reinterpret_cast<ServerPacketHeader*>(buffer);

		auto data = GPacketFactory[header->id]();
		if (data->ParseFromArray(buffer + sizeof(ServerPacketHeader), len - sizeof(ServerPacketHeader)) == false) {
			return nullptr;
		}
		auto pkt = ObjectPool<DBPacket>::MakeShared();
		pkt->header = *header;
		pkt->data = data;

		return pkt;
	}
	static SendBufferRef MakeSendBuffer(DBProtocol::C_LOAD_LOBBY_PLAYERS_INFO& pkt, uint64 targetId, uint64 requestId) { return MakeSendBuffer(pkt, PKT_C_LOAD_LOBBY_PLAYERS_INFO, targetId, requestId); }

private:
	template<typename T>
	static SendBufferRef MakeSendBuffer(T& pkt, uint16 pktId, uint64 targetId, uint64 requestId) {
		const uint16 dataSize = static_cast<uint16>(pkt.ByteSizeLong());
		const uint16 packetSize = dataSize + sizeof(ServerPacketHeader);

		SendBufferRef sendBuffer = GSendBufferManager->Open(packetSize);

		ServerPacketHeader* header = reinterpret_cast<ServerPacketHeader*>(sendBuffer->Buffer());
		header->id = pktId;
		header->size = packetSize;
		header->targetId = targetId;
		header->requestId = requestId;

		pkt.SerializeToArray(&header[1], dataSize);

		sendBuffer->Close(packetSize);
		return sendBuffer;
	}

};

