#include "pch.h"
#include "DeadLockProfiler.h"

/*------------------------------------
  DeadLockProfiler
------------------------------------*/

void DeadLockProfiler::PushLock(const char* name)
{
	int32 lockId = 0;

	LockGuard guard(_lock);
	auto findIt = _nameToId.find(name);
	if (findIt == _nameToId.end()) {
		lockId = static_cast<int32>(_nameToId.size());
		_nameToId[name] = lockId;
		_idToName[lockId] = name;
	}
	else {
		lockId = findIt->second;
	}

	// ��� �ִ� ���� �־��ٸ�
	if (_lockStack.empty() == false) {
		const int32 preId = _lockStack.top();
		if (lockId != preId) {
			set<int32>& history = _lockHistory[preId];
			//  lockId�� ó�� �߰ߵ� �Ŷ��
			if (history.find(lockId) == history.end()) {
				history.insert(lockId);
				CheckCycle();
			}
		}
	}

	_lockStack.push(lockId);
}

void DeadLockProfiler::PopLock(const char* name)
{
	LockGuard guard(_lock);

	if (_lockStack.empty()) {
		CRASH('MULTIPLE_UNLOCK');
	}

	int32 lockId = _nameToId[name];
	if (_lockStack.top() != lockId) {
		CRASH('INVALID_UNLOCK');
	}

	_lockStack.pop();
}

void DeadLockProfiler::CheckCycle()
{
	const int32 lockCount = static_cast<int32>(_nameToId.size());
	_discoveredOrder = vector<int32>(lockCount, -1);
	_discoveredCount = 0;
	_finished = vector<bool>(lockCount, false);
	_parent = vector<int32>(lockCount, -1);

	for (int32 lockId = 0; lockId < lockCount; lockId++) {
		Dfs(lockId);
	}

	_discoveredOrder.clear();
	_finished.clear();
	_parent.clear();
}

void DeadLockProfiler::Dfs(int32 here)
{
	// �̹� �湮�� ��� 
	if (_discoveredOrder[here] != -1) {
		return;
	}

	// �湮�� ������ �ޱ�.
	_discoveredOrder[here] = _discoveredCount++;

	// ��� ������ ������ ��ȸ�Ѵ�
	auto findIt = _lockHistory.find(here);

	// ���� �������� ����� ������ ���� ���
	if (findIt == _lockHistory.end()) {
		_finished[here] = true;
		return;
	}

	set<int32>& nextSet = findIt->second;
	for (int32 there : nextSet) {
		// ���� �湮�� ���� ���ٸ� �湮�Ѵ�
		if (_discoveredOrder[there] == -1) {
			_parent[there] = here;
			Dfs(there);
			continue;;
		}
		
		// here�� there ���� ���� �߰ߵǾ��ٸ�, there�� here�� �ļ��̴�. (������ ����)
		if (_discoveredOrder[there] > _discoveredOrder[here]) {
			continue;
		}

		// ������ �ƴϰ�, Dfs(there)�� ���� �������� �ʾҴٸ�, there�� here�� �����̴�(������ ����)
		if (_finished[here] == false) {
			printf("%s -> %s\n", _idToName[here], _idToName[there]);

			int32 now = here;
			while (true) {
				printf("%s -> %s\n", _idToName[now], _idToName[now]);
				now = _parent[now];
				if (now == there) {
					break;
				}
			}

			CRASH("DEADLOCK_DETECTED");
		}
	}
	_finished[here] = true;

}