#include "pch.h"
#include "DBClientPacketHandler.h"
#include "DBBind.h"
#include "XmlParser.h"
#include "DBSynchronizer.h"
#include "DBProtocol.pb.h"
#include "Struct.pb.h"
#include "DBToProtoMapper.h"

DBPacketHandlerFunc GDBPacketHandler[UINT16_MAX];
PacketFactoryFunc GPacketFactory[UINT16_MAX];

bool Handle_INVALID(PacketSessionRef& session, ServerPacketHeader& header, google::protobuf::Message& data, DBConnection& dbCon)
{
    return false;
}

bool Handle_C_LOAD_LOBBY_PLAYERS_INFO(PacketSessionRef& session, ServerPacketHeader& header, DBProtocol::C_LOAD_LOBBY_PLAYERS_INFO& data, DBConnection& dbConn)
{
	SP::LoadLobbyPlayersInfo sp(dbConn);
	String account = FileUtil::Convert(data.account());
	sp.In_Account((WCHAR*)account.c_str(), static_cast<int32>(account.length()));


	int64 id = 0;
	int32 jobType = 0;
	WCHAR name[100] = {};
	int32 level = 0;

	// 예: DB 칼럼에서 값을 꺼내는 방식 (BindOut 또는 Get 함수를 사용했다고 가정)
	sp.Out_PlayerId(id);
	sp.Out_NickName(name);
	sp.Out_JobType(jobType);
	sp.Out_Level(level);

	DBProtocol::S_LOAD_LOBBY_PLAYERS_INFO result;

	sp.Execute();
	while (sp.Fetch())
	{
		// 3. 결과를 Protocol 메시지에 매핑
		Protocol::LobbyPlayerInfo* info = result.add_infos();
		info->set_id(id);
		info->set_name(FileUtil::Convert(name)); // 문자열 변환 필요
		info->set_jobtype(static_cast<Protocol::JobType>(jobType));
		info->set_level(level);
	}


	SendBufferRef sendBuffer = DBClientPacketHandler::MakeSendBuffer(result, header.targetId, header.requestId);
	session->Send(sendBuffer);


	return true;
}
