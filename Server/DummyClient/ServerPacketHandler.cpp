#include "pch.h"
#include "ServerPacketHandler.h"
#include "BufferReader.h"
#include "Protocol.pb.h"

PacketHandlerFunc GPacketHandler[UINT16_MAX];


bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len)
{
	return false;
}

bool Handle_S_LOGIN(PacketSessionRef& session, Protocol::S_LOGIN& pkt)
{
	cout << "Handle_S_LOGIN : " << pkt.success() << endl;
	if (pkt.success() == false) {
		return false;
	}
	//if (pkt.players().size() == 0) {
	//	// ĳ���� ����
	//}
	Protocol::LobbyPlayerInfo lobbyInfo = pkt.infos(0);
	const uint64 playerId = lobbyInfo.id();
	// ���� UI ��ư ������ ���� ����
	Protocol::C_ENTER_GAME cEnterPkt;
	cEnterPkt.set_playerid(playerId);
	SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(cEnterPkt);
	session->Send(sendBuffer);

	return true;
}

bool Handle_S_ENTER_GAME(PacketSessionRef& session, Protocol::S_ENTER_GAME& pkt)
{
	// TODO.
	return false;
}

bool Handle_S_LEAVE_GAME(PacketSessionRef& session, Protocol::S_LEAVE_GAME& pkt)
{
	return false;
}

bool Handle_S_SPAWN(PacketSessionRef& session, Protocol::S_SPAWN& pkt)
{
	return false;
}

bool Handle_S_DESPAWN(PacketSessionRef& session, Protocol::S_DESPAWN& pkt)
{
	return false;
}

bool Handle_S_MOVE(PacketSessionRef& session, Protocol::S_MOVE& pkt)
{
	return false;
}

bool Handle_S_CHAT(PacketSessionRef& session, Protocol::S_CHAT& pkt)
{
	cout << "msg : " << pkt.msg() << endl;
	return false;
}
