#pragma once

struct CallbackContext
{
	uint64 id;// sessionId or playerId
	std::function<void(void*)> onComplete;
};

// ���� ���� : ���� �� ��� �ÿ� �񵿱�� ó���ϱ� ����
// ������ ���� ��, � ������ ���������� ����ϴ� �����
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