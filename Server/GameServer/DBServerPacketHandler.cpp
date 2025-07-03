#include "pch.h"
#include "DBServerPacketHandler.h"

PacketFactoryFunc GPacketFactory[UINT16_MAX];

bool Handle_S_GET_LOGIN_INFO(PacketSessionRef& session, DBProtocol::S_LOAD_LOBBY_PLAYERS_INFO& pkt)
{
	return false;
}

