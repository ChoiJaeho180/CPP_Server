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
	cout << "success : " << pkt.success() << endl;
	if (pkt.success() == false) {
		return false;
	}

	if (pkt.players().size() == 0) {
		// ĳ���� ����
	}

	// ���� UI ��ư ������ ���� ����
	Protocol::C_ENTER_GAME cEnterPkt;
	cEnterPkt.set_playerindex(0);
	SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(cEnterPkt);
	session->Send(sendBuffer);

	return true;
}

bool Handle_S_ENTER_GAME(PacketSessionRef& session, Protocol::S_ENTER_GAME& pkt)
{
	// TODO.
	return false;
}

bool Handle_S_CHAT(PacketSessionRef& session, Protocol::S_CHAT& pkt)
{
	cout << "msg : " << pkt.msg() << endl;
	return false;
}
