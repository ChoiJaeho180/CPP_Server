#include "pch.h"
#include "ServerPacketHandler.h"
#include "BufferReader.h"
#include "Protocol.pb.h"

PacketHandlerFunc GPacketHandler[UINT16_MAX];

bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len)
{
	return false;
}


bool Handle_S_TEST(PacketSessionRef& sessionRef, Protocol::S_TEST& pkt)
{

	cout << pkt.id() << " " << pkt.hp() << " " << pkt.attack() << " " << endl;

	cout << "Buff Size : " << pkt.buffs_size() << endl;
	for (auto& elem : pkt.buffs()) {
		cout << "buffId : " << elem.buffid() << " " << "remainTime : " << elem.remaintime() << endl;
		cout << "VICTIMS : " << elem.victims_size() << endl;
		for (auto& vic : elem.victims()) {
			cout << "vic :" << vic << endl;
		}
	}

	return false;
}

bool Handle_S_LOGIN(PacketSessionRef& sessionRef, Protocol::S_LOGIN& pkt)
{

	return true;
}
