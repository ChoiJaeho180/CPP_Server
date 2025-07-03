#pragma once

struct CallbackContext
{
	uint64 id;// sessionId or playerId
	std::function<void(void*)> onComplete;
};

// 만든 이유 : 서버 간 통신 시에 비동기로 처리하기 위함
// 응답이 왔을 때, 어떤 로직을 실행할지를 기억하는 저장소
class CallbackStorage
{
public:
	uint64 Add(uint64 id, std::function<void(void*)> callback);
	
	CallbackContext Take(const uint64 requestId);

private:
	USE_LOCK;
	HashMap<uint64, CallbackContext> _contexts;
	std::atomic<uint64> _idGenerator = 1;
};