#pragma once

struct CallbackContext
{
	uint64 id;// sessionId or playerId
	std::function<void(void*)> onComplete;
};
using CallbackContextRef = std::shared_ptr<CallbackContext>;

// ���� ���� : ���� �� ��� �ÿ� �񵿱�� ó���ϱ� ����
// ������ ���� ��, � ������ ���������� ����ϴ� �����
class CallbackStorage
{
public:
	uint64								Add(uint64 id, std::function<void(void*)> callback);
	
	void								TakeBatch(Vector<uint64>& requestIds, OUT HashMap<uint64, CallbackContextRef>& results);

private:
	USE_LOCK;
	HashMap<uint64, CallbackContextRef>	_contexts;
	Atomic<uint64>						_idGenerator = 1;
};