#pragma once

// pre compile header

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
#include "Enum.pb.h"
#include "MathUtils.h"
#include "Protocol.pb.h"
#include "BaseEntity.h"
#include "IUpdate.h"
#include "GameConst.h"
#include "DBProtocol.pb.h"
#include "GameGlobal.h"
#include "DBResponseManager.h"

#include <chrono>

using namespace std::chrono_literals;


using ClientSessionRef		= shared_ptr<class ClientSession>;
using PlayerRef				= shared_ptr<class Player>;
using ZoneRef				= shared_ptr<class Zone>;
using ZoneInstanceRef		= shared_ptr<class ZoneInstance>;
using MonsterSpanwerRef		= shared_ptr<class MonsterSpanwer>;
using BaseEntityRef			= shared_ptr<class BaseEntity>;
using MonsterRef			= shared_ptr<class Monster>;
using GridRef				= shared_ptr<class Grid>;
using DBServerSessionRef	= shared_ptr<class DBServerSession>;

#define SEND_PACKET(pkt)													\
	SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(pkt);	\
	session->Send(sendBuffer);												\

#define SEND_PACKET_TO_DB(pkt)												\
	SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(pkt);	\
	dbSession->Send(sendBuffer);											\

