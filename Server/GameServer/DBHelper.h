#pragma once
#include "../ServerCore/CallbackStorage.h"
#include "DBServerPacketHandler.h"
#include "DBWorkerManager.h"
#include "GameGlobal.h"

class DBHelper
{
public:
	template<typename TRequest, typename TResponse>
	static void SendDBRequest( const uint64 id, TRequest& pkt, const std::function<void(const TResponse&)> onResponse) {
		 const uint64 requestId = GDBServerCallbackMgr->Add<TResponse>(id, [=](const TResponse& respMsg) {
				const TResponse& result = static_cast<const TResponse&>(respMsg);
				onResponse(result);
			});

		const SendBufferRef sendBuffer = DBServerPacketHandler::MakeSendBuffer(pkt, id, requestId);
		DBWorkerManager::GetInstance().AddDBTask(id, sendBuffer);
	}
};

