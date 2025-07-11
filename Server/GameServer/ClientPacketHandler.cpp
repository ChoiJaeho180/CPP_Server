#include "pch.h"
#include "ClientPacketHandler.h"
#include "ClientSession.h"
#include "Player.h"
#include "ObjectUtils.h"
#include "Room.h"
#include "ZoneUtils.h"
#include "CmsManager.h"
#include "MapDesc.h"
#include "PlayerManager.h"
#include "DBServerPacketHandler.h"
#include "DBWorkerManager.h"
#include "DBStruct.pb.h"
#include "DBHelper.h"
#include "ClientSessionManager.h"

PacketHandlerFunc GPacketHandler[UINT16_MAX];

bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len)
{
	return false;
}

bool Handle_C_LOGIN(PacketSessionRef& session, Protocol::C_LOGIN& pkt)
{

	// TODO. 
	// 1. 인증 서버 개발 시 인증 서버에서 redis에 인증 관련 토큰을 저장 및 검증을 한다.
	// 2. 중복 로그인 체크.

	Protocol::S_LOGIN sLoginPkt;
	sLoginPkt.set_success(false);
	std::string account = pkt.account();

	if (account.empty()) {
		SEND_PACKET(sLoginPkt);
		return true;
	}

	ClientSessionRef clientSession = static_pointer_cast<ClientSession>(session);
	
	DBProtocol::C_LOAD_LOBBY_PLAYERS_INFO pktLoadLobbyPlayers;
	pktLoadLobbyPlayers.set_account(account);

	// temp. 현재 캐릭터 정보 가져오도록 구현
	// todo. 간략한 캐릭터 정보만 가져오도록 구현
	DBHelper::SendDBRequest<DBProtocol::C_LOAD_LOBBY_PLAYERS_INFO, DBProtocol::S_LOAD_LOBBY_PLAYERS_INFO>(
		clientSession->GetSessionId(), pktLoadLobbyPlayers,
		[=](const DBProtocol::S_LOAD_LOBBY_PLAYERS_INFO& result) {
			{
				ClientSessionManager::GetInstance().Add(clientSession);
				for (int i = 0; i < result.infos().size(); i++) {
					clientSession->AddLobbyPlayer(result.infos(i));
				}

				Protocol::S_LOGIN sLoginPkt;
				sLoginPkt.mutable_infos()->CopyFrom(result.infos());
				sLoginPkt.set_success(true);

				SEND_PACKET(sLoginPkt);
			}
	});

	return true;
}

bool Handle_C_ENTER_GAME(PacketSessionRef& session, Protocol::C_ENTER_GAME& pkt)
{
	ClientSessionRef clientSession = static_pointer_cast<ClientSession>(session);
	const uint64 playerId = pkt.playerid();

	// todo. 플레이어 정보 가져와서 초기화하기
	const Protocol::LobbyPlayerInfo& info = clientSession->GetLobbyPlayer(playerId);
	PlayerRef curPlayer = ObjectUtils::CreatePlayer(info);
	clientSession->SetCurPlayer(curPlayer);

	////g_Room->DoAsync(&Room::ProcessEnter, curPlayer);

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

	//g_Room->DoAsync(&Room::ProcessLeave, curPlayer);

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

	//g_Room->DoAsync(&Room::ProcessMove, pkt);

	return true;
}

bool Handle_C_CHAT(PacketSessionRef& session, Protocol::C_CHAT& pkt)
{
	Protocol::S_CHAT chatPkt;
	chatPkt.set_msg(pkt.msg());

	SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(chatPkt);
	//cout << "pkt : " << pkt.msg() << endl;
	return false;
}


