#pragma once
#include "DBProtocol.pb.h"

using DBPacketHandlerFunc = std::function<bool(PacketSessionRef&, BYTE*, int32)>;
extern DBPacketHandlerFunc GDBPacketHandler[UINT16_MAX];


enum : uint16 {
{%- for pkt in parser.total_pkt %}
	PKT_{{pkt.name}} = {{pkt.id}},
{%- endfor %}
};

// Custom Handlers
bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len);


{%- for pkt in parser.recv_pkt %}
bool Handle_{{pkt.name}}(PacketSessionRef& session, DBProtocol::{{pkt.name}}& pkt);
{%- endfor %}

class {{output}}
{
public:

	static void Init() {
		for (int i = 0; i < UINT16_MAX; i++) {
			GDBPacketHandler[i] = Handle_INVALID;
		}

{%- for pkt in parser.recv_pkt %}
		GDBPacketHandler[PKT_{{pkt.name}}] = [](PacketSessionRef& session, BYTE* buffer, int32 len) {return HandlePacket<DBProtocol::{{pkt.name}}>(Handle_{{pkt.name}}, session, buffer, len); };
{%- endfor %}
	}

	static bool HandlePacket(PacketSessionRef& session, BYTE* buffer, int32 len) {
		DBPacketHeader* header = reinterpret_cast<DBPacketHeader*>(buffer);
		return GDBPacketHandler[header->id](session, buffer, len);
	}

{%- for pkt in parser.send_pkt %}
	static SendBufferRef MakeSendBuffer(DBProtocol::{{pkt.name}}& pkt, uint64 targetId, uint64 requestId) { return MakeSendBuffer(pkt, PKT_{{pkt.name}}, targetId, requestId); }
{%-endfor %}

private:

	template<typename PacketType, typename ProcessFunc>
	static bool HandlePacket(ProcessFunc func, PacketSessionRef& session, BYTE* buffer, int32 len) {
		PacketType pkt;
		if (pkt.ParseFromArray(buffer + sizeof(DBPacketHeader), len - sizeof(DBPacketHeader)) == false) {
			return false;
		}

		return func(session, pkt);
	}

	template<typename T>
	static SendBufferRef MakeSendBuffer(T& pkt, uint16 pktId, uint64 targetId, uint64 requestId) {
		const uint16 dataSize = static_cast<uint16>(pkt.ByteSizeLong());
		const uint16 packetSize = dataSize + sizeof(DBPacketHeader);

		SendBufferRef sendBuffer = GSendBufferManager->Open(packetSize);

		DBPacketHeader* header = reinterpret_cast<DBPacketHeader*>(sendBuffer->Buffer());
		header->id = pktId;
		header->size = packetSize;
		header->targetId = targetId;
		header->requestId = requestId;

		pkt.SerializeToArray(&header[1], dataSize);

		sendBuffer->Close(packetSize);
		return sendBuffer;
	}

};


