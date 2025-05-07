#include "pch.h"
#include "ClientPacketHandler.h"
#include "BufferReader.h"
#include "Protocol.pb.h"

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

// flat buffer
void ClientPacketHandler::Handle_S_TEST(BYTE* buffer, int32 len)
{
	Protocol::S_TEST pkt;

	// todo. 데이터 변조 체크 필요
	ASSERT_CRASH(pkt.ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)));

	cout << pkt.id() << " " << pkt.hp() << " " << pkt.attack() << " " << endl;

	cout << "Buff Size : " << pkt.buffs_size() << endl;
	for (auto& elem : pkt.buffs()) {
		cout << "buffId : " << elem.buffid() << " " << "remainTime : " << elem.remaintime() << endl;
		cout << "VICTIMS : " << elem.victims_size() << endl;
		for (auto& vic : elem.victims()) {
			cout << "vic :" << vic << endl;
		}
	}
}
