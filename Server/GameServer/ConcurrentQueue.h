#pragma once

#include <mutex>

template<typename T>
class LockQueue
{
private:
	queue<T> _queue;
	mutex _mutex;
	condition_variable _condVar;
public:
	LockQueue() {}
	LockQueue(const T&) = delete;
	T& operator=(const T&) = delete;

	void Push(T value) {
		lock_guard<mutex> m(_mutex);
		_queue.push(value);

		_condVar.notify_one();
	}

	bool TryPop(T& value) {
		lock_guard<mutex> m(_mutex);
		if (_queue.empty() == true) {
			return false;
		}

		value = std::move(_queue.front());
		_queue.pop();
		return true;
	}

	void waitPop(T& value) {
		unique_lock<mutex> m(_mutex);
		_condVar.wait(m, [this] {_queue.empty() == false; });

		value = std::move(_queue.front());
		_queue.pop();
	}
};

