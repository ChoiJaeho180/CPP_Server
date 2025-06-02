#include "pch.h"
#include "ClientPacketHandler.h"
#include "ClientSession.h"
#include "Player.h"

PacketHandlerFunc GPacketHandler[UINT16_MAX];

bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len)
{
	return false;
}

bool Handle_C_LOGIN(PacketSessionRef& session, Protocol::C_LOGIN& pkt)
{
	ClientSessionRef clientSession = static_pointer_cast<ClientSession>(session);
	
	// DB에서 플레이 정보를 긁어온다
	// ClientSession에 플레이 정보를 저장 (메모리)

	static Atomic<uint64> playerIdGen = 1;
	Protocol::S_LOGIN sLoginPkt;
	sLoginPkt.set_success(true);


	{
		PlayerRef player = MakeShared<Player>(playerIdGen++, pkt.name(), Protocol::PlayerType::PLAYER_TYPE_NONE, clientSession);
		clientSession->AddPlayer(player);

		Protocol::PlayerInfo* playerInfo = sLoginPkt.add_players();
		playerInfo->set_id(player->PlayerId());
		playerInfo->set_name(player->Name());
		playerInfo->set_playertype(player->PlayerType());
	}

	{
		PlayerRef player = MakeShared<Player>(playerIdGen++, pkt.name(), Protocol::PlayerType::PLAYER_TYPE_KNIGHT, clientSession);
		clientSession->AddPlayer(player);

		Protocol::PlayerInfo* playerInfo = sLoginPkt.add_players();
		playerInfo->set_id(player->PlayerId());
		playerInfo->set_name(player->Name());
		playerInfo->set_playertype(player->PlayerType());
	}
	

	SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(sLoginPkt);
	clientSession->Send(sendBuffer);
	return true;
}

bool Handle_C_ENTER_GAME(PacketSessionRef& session, Protocol::C_ENTER_GAME& pkt)
{
	ClientSessionRef clientSession = static_pointer_cast<ClientSession>(session);
	uint64 index = pkt.playerindex();
	// todo. validation

	PlayerRef curPlayer = clientSession->GetPlayer(index);
	clientSession->SetCurPlayer(curPlayer);

	
	Protocol::S_ENTER_GAME enterGamePkt;
	enterGamePkt.set_success(true);
	auto sendBuffer = ClientPacketHandler::MakeSendBuffer(enterGamePkt);
	curPlayer->ownerSession().lock()->Send(sendBuffer);

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


