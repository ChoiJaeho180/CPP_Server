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
#include "PacketShardManager.h"
#include "PacketWorker.h"
#include "GameServerSession.h"
#include "DBClientPacketHandler.h"
#include "GenProcedures.h"
#include "DBConst.h"

#pragma comment(lib, "Ws2_32.lib")




void DoNetworkJob(ServerServiceRef& service) {
	while (true) {
		// 네트워크 입 출력 처리 -> 인게임 로직 처리 (패킷 핸들러에 의해)
		service->GetIocpCore()->Dispatch(10);
	}
}

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
	GDBConnectionPool->Push(dbConn);
	
; 
	for (uint16 i = 0; i < DBConst::DB_WORKER_COUNT; i++) {
		const uint16 shardIndex = i;
		GThreadManager->Launch([shardIndex]() {
			PacketWorkerRef worker = MakeShared<PacketWorker>();
			PacketShardManager::GetInstance().AddPacketWorker(shardIndex, worker);
			worker->Tick();
		});
	}

	for (int i = 0; i < DBConst::NETWORK_WORKER_COUNT; i++) {
		GThreadManager->Launch([&dbServer]() {
			DoNetworkJob(dbServer);
		});
	}

	GThreadManager->Join();

	CoreGlobal::Destory();
	return 0;
}