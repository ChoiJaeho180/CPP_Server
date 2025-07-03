#include "pch.h"
#include "CallbackStorage.h"

uint64 CallbackStorage::Add(uint64 id, std::function<void(void*)> callback)
{
	const uint64 requestId = _idGenerator.fetch_add(1);
	CallbackContext ctx;
	ctx.id = id;
	ctx.onComplete = std::move(callback);

	{
		WRITE_LOCK;
		_contexts[requestId] = std::move(ctx);
		cout << "CallbackStorage::Add id : " << id << endl;
		cout << "CallbackStorage::Add requestId : " << requestId << endl;
	}

	return requestId;
}


CallbackContext CallbackStorage::Take(uint64 requestId)
{
	WRITE_LOCK;

	cout << "CallbackStorage::Add requestId : " << requestId << endl;

	auto it = _contexts.find(requestId);
	ASSERT_CRASH(it != _contexts.end());

	CallbackContext ctx = std::move(it->second);
	_contexts.erase(it);

	return ctx;
}
