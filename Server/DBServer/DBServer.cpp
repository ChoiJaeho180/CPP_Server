#include <iostream>
#include "pch.h"
#include "SocketUtils.h"
#include "ThreadManager.h"
#include "Types.h"
#include "Service.h"
#include "Session.h"
#include "BufferWriter.h"
#include "DBBind.h"
#include "XmlParser.h"
#include "DBSynchronizer.h"
#include "CoreGlobal.h"
#include "PacketWorkerManager.h"
#include "PacketWorker.h"
#include "GameServerSession.h"
#include "DBClientPacketHandler.h"
#include "GenProcedures.h"
#include "DBConst.h"
#include "NetWorkerManager.h"

#pragma comment(lib, "Ws2_32.lib")

int main() {
	CoreGlobal::Init();
	DBClientPacketHandler::Init();

	ASSERT_CRASH(GDBConnectionPool->Connect(DBConst::DB_WORKER_COUNT, L"Driver={ODBC Driver 17 for SQL Server};Server=(localdb)\\MSSQLLocalDB;Database=ServerDb;Trusted_Connection=Yes;"));
	ServerServiceRef dbServer = MakeShared<ServerService>(
		ServiceType::Server,
		NetAddress(NetAddress(L"127.0.0.1", 8888)),
		MakeShared<IocpCore>(),
		MakeShared<GameServerSession>,
		5
	);
	ASSERT_CRASH(dbServer->Start());
	DBConnection* dbConn = GDBConnectionPool->Pop();
	DBSynchronizer dbSync(*dbConn);
	dbSync.Synchronize(L"GameDB.xml");
	GDBConnectionPool->Push(dbConn);

	for (uint16 i = 0; i < DBConst::DB_WORKER_COUNT; i++) {
		PacketWorkerRef worker = MakeShared<PacketWorker>();
		PacketWorkerManager::GetInstance().AddWorker(i, worker);

		GThreadManager->Launch([worker]() {
			worker->Init();
			worker->Run();
		});
	}

	for (uint16 i = 0; i < DBConst::NETWORK_WORKER_COUNT; i++) {
		NetWorkerRef worker = MakeShared<NetWorker>();
		NetWorkerManager::GetInstance().AddWorker(i, worker);

		GThreadManager->Launch([&dbServer,worker]() {
			worker->Run(dbServer);
		});
	}

	GThreadManager->Join();

	CoreGlobal::Destory();
	return 0;
}