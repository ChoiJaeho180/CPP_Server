#pragma once
#include "DBServerPacket.h"

using PacketFactoryFunc = std::function<std::shared_ptr<google::protobuf::Message>()>;
extern PacketFactoryFunc GPacketFactory[UINT16_MAX];

using DBPacketHandlerFunc = std::function<bool(PacketSessionRef&, DBServerPacketRef&)>;
extern DBPacketHandlerFunc GDBPacketHandler[UINT16_MAX];


enum : uint16 {
	PKT_C_LOAD_LOBBY_PLAYERS_INFO = 10000,
	PKT_S_LOAD_LOBBY_PLAYERS_INFO = 10001,
};

// Custom Handlers
bool Handle_INVALID(PacketSessionRef& session, ServerPacketHeader& header, google::protobuf::Message& data);
bool Handle_C_LOAD_LOBBY_PLAYERS_INFO(PacketSessionRef& session, ServerPacketHeader& header, DBProtocol::C_LOAD_LOBBY_PLAYERS_INFO& data);

class DBClientPacketHandler
{
public:

	static void Init() {
		for (int i = 0; i < UINT16_MAX; i++) {
			REGISTER_DB_HANDLER(i, google::protobuf::Message, Handle_INVALID);
			GPacketFactory[i] = nullptr;
		}
		REGISTER_DB_HANDLER(PKT_C_LOAD_LOBBY_PLAYERS_INFO, DBProtocol::C_LOAD_LOBBY_PLAYERS_INFO, Handle_C_LOAD_LOBBY_PLAYERS_INFO);
		GPacketFactory[PKT_C_LOAD_LOBBY_PLAYERS_INFO] = []() { return ObjectPool<DBProtocol::C_LOAD_LOBBY_PLAYERS_INFO>::MakeShared(); };
	}

	static std::optional<DBServerPacketRef> ParsePacket(BYTE* buffer, int32 len)
	{
		ServerPacketHeader* header = reinterpret_cast<ServerPacketHeader*>(buffer);
		auto data = GPacketFactory[header->id]();
		if (!data || data->ParseFromArray(buffer + sizeof(ServerPacketHeader), len - sizeof(ServerPacketHeader)) == false) {
			return std::nullopt;
		}

		DBServerPacketRef pkt = ObjectPool<DBServerPacket>::MakeShared();
		pkt->header = *header;
		pkt->data = data;

		return pkt;
	}

	static bool HandlePacket(PacketSessionRef session, DBServerPacketRef pkt) {
		return GDBPacketHandler[pkt->header.id](session, pkt);
	}
	static SendBufferRef MakeSendBuffer(DBProtocol::S_LOAD_LOBBY_PLAYERS_INFO& pkt, uint64 targetId, uint64 requestId) { return MakeSendBuffer(pkt, PKT_S_LOAD_LOBBY_PLAYERS_INFO, targetId, requestId); }

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

