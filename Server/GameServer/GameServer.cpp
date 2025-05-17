#include "pch.h"
#include "SocketUtils.h"
#include "ThreadManager.h"
#include "Types.h"
#include "Service.h"
#include "Session.h"
#include "ClientSession.h"
#include "ClientSessionManager.h"
#include "BufferWriter.h"
#include "ClientPacketHandler.h"
#include <tchar.h>
#include "Protocol.pb.h"
#include "Room.h"
#include "Player.h"

#pragma comment(lib, "Ws2_32.lib")

enum {
	// TODO. 런타임에 경과를 지켜보고 보정하도록 수정
	WORKER_TICK = 64
};
void DoWokerJob(ServerServiceRef& service) {

	while (true) {
		
		LEndTickCount = ::GetTickCount64() + WORKER_TICK;

		// 네트워크 입 출력 처리 -> 인게임 로직 처리 (패킷 핸들러에 의해)
		service->GetIocpCore()->Dispatch(10);

		// 예약된 task 처리
		ThreadManager::ProcessReservedTasks();

		// 글로벌 큐 
		ThreadManager::ProcessGlobalQueue();
	}
}


int main()
{
	/*GRoom->DoTimer(1000, []() {cout << "1000!!!!" << endl; });
	GRoom->DoTimer(2000, []() {cout << "2000!!!!" << endl; });
	GRoom->DoTimer(3000, []() {cout << "3000!!!" << endl; });*/

	ASSERT_CRASH(GDBConnectionPool->Connect(1, L"Driver={ODBC Driver 17 for SQL Server};Server=(localdb)\\MSSQLLocalDB;Database=ServerDb;Trusted_Connection=Yes;"));
	
	// Create Table
	{
		auto query = L"									\
			DROP TABLE IF EXISTS [dbo].[Gold]			\
			CREATE TABLE [dbo].[Gold]					\
			(											\
				[id] INT NOT NULL PRIMARY KEY IDENTITY,	\
				[gold] INT NULL							\
			);";

		DBConnection* dbConn = GDBConnectionPool->Pop();
		ASSERT_CRASH(dbConn->Execute(query));
		GDBConnectionPool->Push(dbConn);
	}

	for (int32 i = 0; i < 3; i++) {
		DBConnection* dbConn = GDBConnectionPool->Pop();
		dbConn->Unbind();

		int gold = 100 + i * 20;
		SQLLEN len = 0;

		ASSERT_CRASH(dbConn->BindParam(1, SQL_C_LONG, SQL_INTEGER, sizeof(gold), &gold, &len));
		ASSERT_CRASH(dbConn->Execute(L"INSERT INTO [dbo].[Gold]([gold]) VALUES(?)"));

		GDBConnectionPool->Push(dbConn);
	}
	

	{
		DBConnection* dbConn = GDBConnectionPool->Pop();
		dbConn->Unbind();

		int32 gold = 100;
		SQLLEN len = 0;
		ASSERT_CRASH(dbConn->BindParam(1, SQL_C_LONG, SQL_INTEGER, sizeof(gold), &gold, &len));

		int32 outId = 0;
		SQLLEN  outIdLen = 0;
		ASSERT_CRASH(dbConn->BIndCol(1, SQL_C_LONG, sizeof(outId), &outId, &outIdLen));

		int outGold = 0;
		SQLLEN outGoldLen = 0;

		ASSERT_CRASH(dbConn->BIndCol(2, SQL_C_LONG, sizeof(outGold), &outGold, &outGoldLen));

		ASSERT_CRASH(dbConn->Execute(L"SELECT id, gold FROM [dbo].[Gold] WHERE gold >= (?);"));
		while (dbConn->Fetch()) {
			cout << "Id : " << outId << " Gold : " << outGold << endl;
		}

		GDBConnectionPool->Push(dbConn);
	}
	
	ClientPacketHandler::Init();

	ServerServiceRef service = MakeShared<ServerService>(
		NetAddress(NetAddress(L"127.0.0.1", 7777)),
		MakeShared<IocpCore>(),
		MakeShared<ClientSession>,
		100
	);

	ASSERT_CRASH(service->Start());
	
	for (int i = 0; i < 5; i++) {
		GThreadManager->Launch([&service]() {
			DoWokerJob(service);
		});
	}
	
	DoWokerJob(service);

	GThreadManager->Join();
}