#include "pch.h"
#include "ClientPacketHandler.h"
#include "ClientSession.h"
#include "Player.h"
#include "ObjectUtils.h"
#include "Room.h"

PacketHandlerFunc GPacketHandler[UINT16_MAX];

bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len)
{
	return false;
}

bool Handle_C_LOGIN(PacketSessionRef& session, Protocol::C_LOGIN& pkt)
{
	// TODO. DB에서 account 정보를 긁어온다.
	// TODO. 인증 서버 개발 시 인증 서버에서 redis에 인증 관련 토큰을 저장하고
	//  여기서 검증을 한다.

	ClientSessionRef clientSession = static_pointer_cast<ClientSession>(session);
	
	static Atomic<uint64> playerIdGen = 1;

	PlayerRef player = MakeShared<Player>(pkt.name(), Protocol::PlayerType::PLAYER_TYPE_NONE, clientSession);
	clientSession->AddPlayer(player);

	{
		Protocol::S_LOGIN sLoginPkt;

		sLoginPkt.set_success(true);

		SEND_PACKET(sLoginPkt);
	}
	return true;
}

bool Handle_C_ENTER_GAME(PacketSessionRef& session, Protocol::C_ENTER_GAME& pkt)
{
	ClientSessionRef clientSession = static_pointer_cast<ClientSession>(session);
	PlayerRef curPlayer = ObjectUtils::CreatePlayer(clientSession);

	Room::GetInstance().DoAsync(&Room::ProcessEnter, curPlayer);
	
	return true;
}

bool Handle_C_LEAVE_GAME(PacketSessionRef& session, Protocol::C_LEAVE_GAME& pkt)
{
	ClientSessionRef clientSession = static_pointer_cast<ClientSession>(session);
	if (clientSession == nullptr) {
		return false;
	}

	PlayerRef curPlayer = clientSession->GetCurPlayer();
	if (curPlayer == nullptr) {
		return false;
	}

	Room::GetInstance().DoAsync(&Room::ProcessLeave, curPlayer);

	return true;
}

bool Handle_C_SPAWN(PacketSessionRef& session, Protocol::C_SPAWN& pkt)
{
	return false;
}

bool Handle_C_MOVE(PacketSessionRef& session, Protocol::C_MOVE& pkt)
{
	ClientSessionRef clientSession = static_pointer_cast<ClientSession>(session);
	if (clientSession == nullptr) {
		return false;
	}

	PlayerRef curPlayer = clientSession->GetCurPlayer();
	if (curPlayer == nullptr) {
		return false;
	}

	Room::GetInstance().DoAsync(&Room::ProcessMove, pkt);

	return true;
}

bool Handle_C_CHAT(PacketSessionRef& session, Protocol::C_CHAT& pkt)
{
	Protocol::S_CHAT chatPkt;
	chatPkt.set_msg(pkt.msg());

	SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(chatPkt);
	cout << "pkt : " << pkt.msg() << endl;
	return false;
}


