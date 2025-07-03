#pragma once

#ifdef _DEBUG
#pragma comment(lib, "ServerCore\\Debug\\ServerCore.lib")
#pragma comment(lib, "Protobuf\\Debug\\libprotobufd.lib")
#else
#pragma comment(lib, "ServerCore\\Release\\ServerCore.lib")
#pragma comment(lib, "Protobuf\\Release\\libprotobuf.lib")
#endif
#include <winsock2.h>
#include <mswsock.h>
#include <ws2tcpip.h>

#include "CorePch.h"


using PacketWorkerRef				= shared_ptr<class PacketWorker>;
using GameServerSessionRef			= shared_ptr<class GameServerSession>;

#define REGISTER_DB_HANDLER(PKT_ID, TYPE, FUNC)												\
	GDBPacketHandler[PKT_ID] = [](PacketSessionRef& session, DBServerPacketRef& pkt) {		\
		DBConnection* dbConn = GDBConnectionPool->Pop();									\
		bool result = false;																\
		if constexpr (std::is_same_v<TYPE, google::protobuf::Message>) {					\
			result = FUNC(session, pkt->header, *pkt->data, *dbConn);						\
		} else {																			\
			auto typed = std::static_pointer_cast<TYPE>(pkt->data);							\
			result = FUNC(session, pkt->header, *typed, *dbConn);							\
		}																					\
		GDBConnectionPool->Push(dbConn);													\
		return result;																		\
	};