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
		return PopNoLock();
	}

	T PopNoLock() {
		if (_items.empty()) {
			return T();
		}

		T result = _items.front();
		_items.pop();
		return result;
	}

	void PopAll(OUT Vector<T>& items) {
		Queue<T> copyItems;
		{
			WRITE_LOCK;
			std::swap(copyItems, _items);
		}

		while (copyItems.empty() == false) {
			T result = copyItems.front();
			copyItems.pop();

			items.push_back(result);
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