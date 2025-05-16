#pragma once

template<typename T>
class LockQueue
{
public:
	void Push(T task) {
		WRITE_LOCK;
		_items.push(task);
	}

	T Pop() {
		WRITE_LOCK;
		if (_items.empty()) {
			return T();
		}

		T result = _items.front();
		_items.pop();
		return result;
	}

	void PopAll(OUT Vector<T>& items) {
		WRITE_LOCK;

		while (T item = Pop()) {
			items.push_back(item);
		}
	}

	void Clear() {
		WRITE_LOCK;
		_items = Queue<T>();
	}

private:
	USE_LOCK;
	Queue<T> _items;
};