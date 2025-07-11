#pragma once
#include "DBServerPacket.h"

using PacketFactoryFunc = std::function<std::shared_ptr<google::protobuf::Message>()>;
extern PacketFactoryFunc GPacketFactory[UINT16_MAX];

using DBPacketHandlerFunc = std::function<bool(PacketSessionRef&, DBServerPacketRef&)>;
extern DBPacketHandlerFunc GDBPacketHandler[UINT16_MAX];


enum : uint16 {
{%- for pkt in parser.total_pkt %}
	PKT_{{pkt.name}} = {{pkt.id}},
{%- endfor %}
};

// Custom Handlers
bool Handle_INVALID(PacketSessionRef& session, ServerPacketHeader& header, google::protobuf::Message& data);


{%- for pkt in parser.recv_pkt %}
bool Handle_{{pkt.name}}(PacketSessionRef& session, ServerPacketHeader& header, DBProtocol::{{pkt.name}}& data);
{%- endfor %}

class {{output}}
{
public:

	static void Init() {
		for (int i = 0; i < UINT16_MAX; i++) {
			REGISTER_DB_HANDLER(i, google::protobuf::Message, Handle_INVALID);
			GPacketFactory[i] = nullptr;
		}
{%- for pkt in parser.recv_pkt %}
		REGISTER_DB_HANDLER(PKT_{{pkt.name}}, DBProtocol::{{pkt.name}}, Handle_{{pkt.name}});
		GPacketFactory[PKT_{{pkt.name}}] = []() { return ObjectPool<DBProtocol::{{pkt.name}}>::MakeShared(); };
{%- endfor %}
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

{%- for pkt in parser.send_pkt %}
	static SendBufferRef MakeSendBuffer(DBProtocol::{{pkt.name}}& pkt, uint64 targetId, uint64 requestId) { return MakeSendBuffer(pkt, PKT_{{pkt.name}}, targetId, requestId); }
{%-endfor %}

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


