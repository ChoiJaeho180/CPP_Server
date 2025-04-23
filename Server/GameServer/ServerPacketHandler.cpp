#include "pch.h"
#include "ServerPacketHandler.h"
#include "BufferReader.h"
#include "BufferWriter.h"

void ServerPacketHandler::HandlePacket(BYTE* buffer, int32 len)
{
	BufferReader br(buffer, len);

	PacketHeader header;
	br.Peek(&header);

	switch (header.id) {

	}
}

SendBufferRef ServerPacketHandler::Make_S_TEST(uint64 id, uint32 hp, uint16 attack, vector<BuffData> buffs)
{
	SendBufferRef sendBuffRef = GSendBufferManager->Open(4096);
	BufferWriter bw(sendBuffRef->Buffer(), sendBuffRef->AllocSize());

	PacketHeader* header = bw.Reserve<PacketHeader>();
	bw << id << hp << attack;
	bw << (uint16)buffs.size();
	for (BuffData& buff : buffs) {
		bw << buff.buffId << buff.remainTime;
	}

	header->size = bw.WriteSize();
	header->id = S_TEST; // 1 : Hello Msg

	sendBuffRef->Close(bw.WriteSize());

	return sendBuffRef;
}
