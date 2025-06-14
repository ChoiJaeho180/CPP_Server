﻿#include "pch.h"
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
#include "Player.h"
#include "DBBind.h"
#include "XmlParser.h"
#include "DBSynchronizer.h"
#include "GenProcedures.h"
#include "CoreGlobal.h"
#include "ZoneManager.h"
#include "ZoneDesc.h"
#include "CmsManager.h"
#include <thread>

#pragma comment(lib, "Ws2_32.lib")


enum {
	// TODO. 런타임에 경과를 지켜보고 보정하도록 수정
	WORKER_TICK = 64
};

void DoWokerJob(ServerServiceRef& service) {
	int a = 0;
	while (true) {
		const uint64 startTick = GetTickCount64();
		LEndTickCount = startTick + WORKER_TICK;

		// 네트워크 입 출력 처리 -> 인게임 로직 처리 (패킷 핸들러에 의해)
		service->GetIocpCore()->Dispatch(10);

		// zone의 업데이트 요청 실제 작업을 하진 않음
		ZoneManager::GetInstance().EnqueueUpdates();

		// 예약된 task 처리
		ThreadManager::ProcessReservedTasks();

		// 글로벌 큐 
		ThreadManager::ProcessGlobalQueue();

		const uint64 elapsed = GetTickCount64() - startTick;
		if (elapsed > WORKER_TICK) {
			printf("Tick too slow: {%llu}ms\n", elapsed);
		}
	}
}

int main()
{
	/*GRoom->DoTimer(1000, []() {cout << "1000!!!!" << endl; });
	GRoom->DoTimer(2000, []() {cout << "2000!!!!" << endl; });
	GRoom->DoTimer(3000, []() {cout << "3000!!!" << endl; });*/
	
	/*ASSERT_CRASH(GDBConnectionPool->Connect(1, L"Driver={ODBC Driver 17 for SQL Server};Server=(localdb)\\MSSQLLocalDB;Database=ServerDb;Trusted_Connection=Yes;"));
	DBConnection* dbConn = GDBConnectionPool->Pop();
	DBSynchronizer dbSync(*dbConn);
	dbSync.Synchronize(L"GameDB.xml");

	{
		SP::GetGold getGold(*dbConn);
		getGold.In_Gold(100);

		int32 id = 0;
		int32 gold = 0;
		WCHAR name[100];
		TIMESTAMP_STRUCT date;

		getGold.Out_Id(OUT id);

		getGold.Out_Gold(OUT gold);

		getGold.Out_CreateDate(OUT date);

		getGold.Out_Name(OUT name);

		getGold.Execute();
		while (getGold.Fetch()) {
			GConsoleLogger->WriteStdOut(Color::BLUE,
				L"ID[%d] Gold[%d] Name[%s]\n", id, gold, name
				);
		}
	}*/

	CoreGlobal::Init();
	CmsManager::GetInstance().Init("../Cms/");
	ClientPacketHandler::Init();
	ZoneManager::GetInstance().Init();

	ServerServiceRef service = MakeShared<ServerService>(
		NetAddress(NetAddress(L"127.0.0.1", 7777)),
		MakeShared<IocpCore>(),
		MakeShared<ClientSession>,
		100
	);

	ASSERT_CRASH(service->Start());

	//unsigned int MAX_WORKER_THREADS = std::thread::hardware_concurrency(); 
	for (int i = 0; i < 1; i++) {
		GThreadManager->Launch([&service]() {
			DoWokerJob(service);
		});
	}

	//DoWokerJob(service);

	GThreadManager->Join();

	CoreGlobal::Destory();
}