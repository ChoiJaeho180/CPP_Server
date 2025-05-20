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
#include "DBBind.h"

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
				[gold] INT NULL,						\
				[name] NVARCHAR(50) NULL,				\
				[createDate] DATETIME NULL				\
			);";

		DBConnection* dbConn = GDBConnectionPool->Pop();
		ASSERT_CRASH(dbConn->Execute(query));
		GDBConnectionPool->Push(dbConn);
	}

	for (int32 i = 0; i < 3; i++) {
		DBConnection* dbConn = GDBConnectionPool->Pop();
		DBBind<3, 0> dbBind(*dbConn, L"INSERT INTO [dbo].[Gold]([gold], [name], [createDate]) VALUES(?, ?, ?)");
		int gold = 100 + i * 20;
		dbBind.BindParam(0, gold);

		WCHAR name[100] = L"단결";
		dbBind.BindParam(1, name);
		TIMESTAMP_STRUCT createDate = {2022, 10, 3};
		dbBind.BindParam(2, createDate);

		ASSERT_CRASH(dbBind.Execute());
		/*dbConn->Unbind();

		int gold = 100 + i * 20;
		SQLLEN len = 0;

		WCHAR name[100] = L"단결";
		SQLLEN nameLen = 0;

		TIMESTAMP_STRUCT createDate = {};
		SQLLEN dateLen = 0;
		createDate.year = 2022;
		createDate.month = 10;
		createDate.day = 3;

		ASSERT_CRASH(dbConn->BindParam(1, &gold, &len));
		ASSERT_CRASH(dbConn->BindParam(2, name, &nameLen));
		ASSERT_CRASH(dbConn->BindParam(3, &createDate, &dateLen));

		ASSERT_CRASH(dbConn->Execute(L"INSERT INTO [dbo].[Gold]([gold], [name], [createDate]) VALUES(?, ?, ?)"));*/

		GDBConnectionPool->Push(dbConn);
	}
	

	{
		DBConnection* dbConn = GDBConnectionPool->Pop();
		DBBind<1, 4> dbBind(*dbConn, L"SELECT id, gold, name, createDate FROM [dbo].[Gold] WHERE gold >= (?)");
		int32 gold = 100;
		dbBind.BindParam(0, gold);
		int32 outId = 0;
		int32 outGold = 0;
		WCHAR outName[100];
		TIMESTAMP_STRUCT coutDate = {};
		dbBind.BindCol(0, OUT outId);
		dbBind.BindCol(1, OUT outGold);
		dbBind.BindCol(2, OUT outName);
		dbBind.BindCol(3, OUT coutDate);

		ASSERT_CRASH(dbBind.Execute());
		/*dbConn->Unbind();

		int32 gold = 100;
		SQLLEN len = 0;
		ASSERT_CRASH(dbConn->BindParam(1, &gold, &len));

		int32 outId = 0;
		SQLLEN  outIdLen = 0;
		ASSERT_CRASH(dbConn->BindCol(1, &outId, &outIdLen));
		

		int outGold = 0;
		SQLLEN outGoldLen = 0;

		ASSERT_CRASH(dbConn->BindCol(2, &outGold, &outGoldLen));

		WCHAR outName[100];
		SQLLEN outNameLen = 0;
		ASSERT_CRASH(dbConn->BindCol(3, outName, len32(outName), &outNameLen));

		TIMESTAMP_STRUCT coutDate = {};
		SQLLEN dateLen = 0;
		ASSERT_CRASH(dbConn->BindCol(4, &coutDate,  &dateLen));

		ASSERT_CRASH(dbConn->Execute(L"SELECT id, gold, name, createDate FROM [dbo].[Gold] WHERE gold >= (?);"));*/
		wcout.imbue(locale("kor"));
		while (dbConn->Fetch()) {
			wcout << " Id : " << outId << " Gold : " << outGold <<  " name : " << outName << endl;
			wcout << " year : " << coutDate.year << " month : " << coutDate.month << " day : " << coutDate.day << endl;
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