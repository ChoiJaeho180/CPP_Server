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

	// 잡고 있는 락이 있었다면
	if (_lockStack.empty() == false) {
		const int32 preId = _lockStack.top();
		if (lockId != preId) {
			set<int32>& history = _lockHistory[preId];
			//  lockId가 처음 발견된 거라면
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
	// 이미 방문한 경우 
	if (_discoveredOrder[here] != -1) {
		return;
	}

	// 방문한 순번을 메김.
	_discoveredOrder[here] = _discoveredCount++;

	// 모든 인접한 정점을 순회한다
	auto findIt = _lockHistory.find(here);

	// 현재 정점에서 연결된 간선이 없는 경우
	if (findIt == _lockHistory.end()) {
		_finished[here] = true;
		return;
	}

	set<int32>& nextSet = findIt->second;
	for (int32 there : nextSet) {
		// 아직 방문한 적이 없다며 방문한다
		if (_discoveredOrder[there] == -1) {
			_parent[there] = here;
			Dfs(there);
			continue;;
		}
		
		// here가 there 보다 먼저 발견되었다면, there는 here의 후손이다. (순방향 간선)
		if (_discoveredOrder[there] > _discoveredOrder[here]) {
			continue;
		}

		// 순방향 아니고, Dfs(there)가 아직 종료하지 않았다면, there는 here의 선조이다(역방향 간선)
		if (_finished[there] == false) {
			printf("%s -> %s\n", _idToName[here], _idToName[there]);

			int32 now = here;
			while (true) {
				printf("%s -> %s\n", _idToName[_parent[now]], _idToName[now]);
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
