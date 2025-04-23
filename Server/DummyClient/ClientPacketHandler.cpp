#include "pch.h"
#include "ClientPacketHandler.h"
#include "BufferReader.h"

void ClientPacketHandler::HandlePacket(BYTE* buffer, int32 len)
{
	BufferReader reader(buffer, len);

	PacketHeader header;
	reader >> header;

	switch (header.id) {
	case S_TEST:
		Handle_S_TEST(buffer, len);
		break;
	}
}

struct BuffData {
	uint64 buffId;
	float remainTime;
};

struct S_TEST {
	uint64 id;
	uint32 hp;
	uint16 attack;
	vector<BuffData> buffs;
};

void ClientPacketHandler::Handle_S_TEST(BYTE* buffer, int32 len)
{
	BufferReader reader(buffer, len);

	PacketHeader header;
	reader >> header;

	uint64 id;
	uint32 hp;
	uint16 attack;
	reader >> id >> hp >> attack;

	cout << " Id : " << id << "  hp : " << hp << "  att :" << attack << endl;
	vector<BuffData> buffs;
	uint16 buffCount;
	reader >> buffCount;

	buffs.resize(buffCount);
	for (int32 i = 0; i < buffCount; i++) {
		reader >> buffs[i].buffId >> buffs[i].remainTime;
	}

	cout << "BuffCount : " << buffCount << endl;
	for (int32 i = 0; i < buffCount; i++) {
		cout << "BuffInfo : " << buffs[i].buffId << " " << buffs[i].remainTime << endl;
	}
}
