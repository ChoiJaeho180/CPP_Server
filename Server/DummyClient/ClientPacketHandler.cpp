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
#pragma pack(1)
struct PKT_S_TEST {
	struct BuffListItem {
		uint64 buffId;
		float remainTime;

		uint16 victimsOffset;
		uint16 victimsCount;

		bool Validate(BYTE* packetStart, uint16 packetSize, OUT uint32& size) {
			if (victimsOffset + victimsCount * sizeof(uint64) > packetSize) {
				return false;
			}

			size += victimsCount * sizeof(uint64);
			return true;
		}
	};

	uint16 packetSize;
	uint16 packetId;
	uint64 id;
	uint32 hp;
	uint16 attack;

	uint16 buffsOffset;
	uint16 buffsCount;

	bool Validate() {
		uint32 size = 0;
		
		size += sizeof(PKT_S_TEST);
		if (packetSize < size) {
			return false;
		}
		
		if (buffsOffset + buffsCount * sizeof(BuffListItem) > packetSize) {
			return false;
		}
		// Buffers 가변 데이터 크기 추가
		size += buffsCount * sizeof(BuffListItem);

		BuffsList buffsList = GetBuffsList();

		for (PKT_S_TEST::BuffListItem& elem : buffsList) {
			if (elem.Validate(reinterpret_cast<BYTE*>(this), packetSize, size) == false) {
				return false;
			}
		}

		// 최종 크기 비교
		if (size != packetSize) {
			return false;
		}

		return true;
	}

	using BuffsList = PacketList<PKT_S_TEST::BuffListItem>;
	using BuffsVictimesList = PacketList<uint64>;

	BuffsList GetBuffsList() {
		BYTE* data = reinterpret_cast<BYTE*>(this);
		data += buffsOffset;

		return BuffsList(reinterpret_cast<PKT_S_TEST::BuffListItem*>(data), buffsCount);
	}

	BuffsVictimesList GetBuffsVictimList(BuffListItem* item) {
		BYTE* data = reinterpret_cast<BYTE*>(this);
		data += item->victimsOffset;

		return BuffsVictimesList(reinterpret_cast<uint64*>(data), item->victimsCount);
	}

	/*vector<BuffData> buffs;
	wstring name;*/
};
#pragma pack()

// flat buffer
void ClientPacketHandler::Handle_S_TEST(BYTE* buffer, int32 len)
{
	BufferReader reader(buffer, len);


	PKT_S_TEST* pkt = reinterpret_cast<PKT_S_TEST*>(buffer);

	if (pkt->Validate() == false) {
		return;
	}
	
	PKT_S_TEST::BuffsList buffs = pkt->GetBuffsList();
	              
	cout << "BuffCout : " << buffs.Count() << endl;
	

	for (auto& buff : buffs) {
		cout << "BufInfo : " << buff.buffId << " " << buff.remainTime << endl;
		PKT_S_TEST::BuffsVictimesList victims = pkt->GetBuffsVictimList(&buff);
		for (auto& victim : victims) {
			cout << "Victim : " << victim << endl;
		}
	}
}
