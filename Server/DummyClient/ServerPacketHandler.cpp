#include "pch.h"
#include "ServerPacketHandler.h"
#include "BufferReader.h"
#include "Protocol.pb.h"

PacketHandlerFunc GPacketHandler[UINT16_MAX];

bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len)
{
	return false;
}



bool Handle_S_LOGIN(PacketSessionRef& sessionRef, Protocol::S_LOGIN& pkt)
{

	return true;
}

bool Handle_S_ENTER_GAME(PacketSessionRef& sessionRef, Protocol::S_ENTER_GAME& pkt)
{
	return false;
}

bool Handle_S_CHAT(PacketSessionRef& sessionRef, Protocol::S_CHAT& pkt)
{
	return false;
}
