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
{%- for pkt in parser.total_pkt %}
	PKT_{{pkt.name}} = {{pkt.id}},
{%- endfor %}
};

class {{output}}
{
public:

	static void Init() {
		for (int i = 0; i < UINT16_MAX; i++) {
			GPacketFactory[i] = nullptr;
		}

{%- for pkt in parser.recv_pkt %}
		GPacketFactory[PKT_{{pkt.name}}] = []() { return ObjectPool<DBProtocol::{{pkt.name}}>::MakeShared(); };
{%- endfor %}
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


