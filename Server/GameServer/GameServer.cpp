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
#include "Player.h"
#include "DBBind.h"
#include "XmlParser.h"
#include "DBSynchronizer.h"
#include "CoreGlobal.h"
#include "ZoneManager.h"
#include "CmsManager.h"
#include "CmsLookup.h"
#include "DBServerSession.h"
#include "DBWorkerManager.h"
#include "DBServerPacketHandler.h"

#pragma comment(lib, "Ws2_32.lib")


enum {
	// TODO. 런타임에 경과를 지켜보고 보정하도록 수정
	WORKER_TICK = 64
};

void DoGameWorkerJob(ServerServiceRef& service) {
	while (true) {
		const uint64 startTick = GetTickCount64();
		LEndTickCount = startTick + WORKER_TICK;

		// 네트워크 입 출력 처리 -> 인게임 로직 처리 (패킷 핸들러에 의해)
		service->GetIocpCore()->Dispatch(10);

		// zone의 업데이트 요청 실제 작업을 하진 않음. 
		// -> 하나의 thread가 다 처리하지 않도록 분산
		ZoneManager::GetInstance().EnqueueUpdates();

		// 예약된 task 처리
		ThreadManager::ProcessReservedTasks();

		// 글로벌 큐 
		ThreadManager::ProcessGlobalQueue();

		// DB response 
		GDBServerCallbackMgr->ProcessPackets();

		const uint64 elapsed = GetTickCount64() - startTick;
		if (elapsed > WORKER_TICK) {
			printf("Tick too slow: {%llu}ms\n", elapsed);
		}
	}
}

int main()
{

	CoreGlobal::Init();
	GameGlobal::Init();

	CmsManager::GetInstance().Init("../Cms/");
	CmsLookup::Build();
	ClientPacketHandler::Init();
	DBServerPacketHandler::Init();

	ZoneManager::GetInstance().Init();

	ServerServiceRef gameServer = MakeShared<ServerService>(
		ServiceType::Server,
		NetAddress(NetAddress(L"127.0.0.1", 7777)),
		MakeShared<IocpCore>(),
		MakeShared<ClientSession>,
		100
	);
	ASSERT_CRASH(gameServer->Start());
	//unsigned int MAX_WORKER_THREADS = std::thread::hardware_concurrency(); 
	for (int i = 0; i < GameConst::GAME_WORKER_COUNT; i++) {
		GThreadManager->Launch([&gameServer]() {
			DoGameWorkerJob(gameServer);
			});
	}

	ClientServiceRef dbClient = MakeShared<ClientService>(
		NetAddress(L"127.0.0.1", 8888),
		MakeShared<IocpCore>(),
		MakeShared<DBServerSession>,
		1
	);

	ASSERT_CRASH(dbClient->Start());

	for (int i = 0; i < GameConst::DB_WORKER_COUNT; i++) {
		int shardId = i;
		GThreadManager->Launch([&dbClient, shardId]() {
			DBWorkerRef worker = MakeShared<DBWorker>(shardId);
			DBWorkerManager::GetInstance().AddWorker(shardId, worker);

			worker->Tick(dbClient);
		});
	}
	

	//DoGameWorkerJob(gameServer);

	GThreadManager->Join();

	CoreGlobal::Destory();
}