#include "pch.h"
#include "CallbackStorage.h"

uint64 CallbackStorage::Add(uint64 id, std::function<void(void*)> callback)
{
	const uint64 requestId = _idGenerator.fetch_add(1);
	CallbackContextRef ctx = ObjectPool<CallbackContext>::MakeShared();
	ctx->id = id;
	ctx->onComplete = std::move(callback);

	{
		WRITE_LOCK;
		_contexts[requestId] = std::move(ctx);
		cout << "CallbackStorage::Add id : " << id << endl;
		cout << "CallbackStorage::Add requestId : " << requestId << endl;
	}

	return requestId;
}

void CallbackStorage::TakeBatch(Vector<uint64>& requestIds,  OUT HashMap<uint64, CallbackContextRef>& results)
{
	results.reserve(requestIds.size());

	WRITE_LOCK;

	for (int i = 0; i < requestIds.size(); i++) {
		const uint64& requestId = requestIds[i];

		cout << "CallbackStorage::Add requestId : " << requestId << endl;

		auto it = _contexts.find(requestId);
		ASSERT_CRASH(it != _contexts.end());

		results[requestId] = std::move(it->second);
		_contexts.erase(it);
	}
}
