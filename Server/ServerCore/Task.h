#pragma once
#include <functional>

using CallbackType = std::function<void()>;

class Task
{
public:
	Task(CallbackType&& callback) : _callback(std::move(callback)) {

	}

	template<typename T, typename Ret, typename... Args>
	Task(weak_ptr<T> weakOwner, Ret(T::* memFunc), Args&&... args) {

		_callback = [weakOwner, memFunc, args ...]() {
			if (auto owner = weakOwner.lock()) {
				(owner.get()->*memFunc)(args ...);
			}
		};
	}

	void Execute() {
		_callback();
	}
private:
	CallbackType _callback;
};

