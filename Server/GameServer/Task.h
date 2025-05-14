#pragma once
#include <functional>


// ÇÔ¼öÀÚ


class ITask
{
public:
	virtual void Execute() {}
};

template<typename Ret, typename ... Args>
class FuncTask  : public ITask
{
	using FuncType = Ret(*)(Args ...);
public:
	FuncTask(FuncType func, Args ...args) : _func(func), _tuple(args ...) {

	}

	Ret operator()(Args ...args) {
		std::apply(_func, _tuple);
	}

	virtual void Execute() override {
		std::apply(_func, _tuple); // c++17
	}

private:
	FuncType _func;
	std::tuple<Args ...> _tuple;
};

template<typename T, typename Ret, typename ... Args>
class MemberTask : public ITask
{
	using FuncType = Ret(T::*)(Args ...);
public:
	MemberTask(T* obj, FuncType func, Args... args) : _obj(obj), _func(func), _tuple(args ...) {

	}


	virtual void Execute() override {
		std::apply([this](Args... args) { (_obj->*_func)(args...); }, _tuple); // c++17
	}

private:
	T*						_obj;
	FuncType				_func;
	std::tuple<Args ...>	_tuple;
};




using TaskRef = shared_ptr<ITask>;

class TaskQueue 
{
public:
	void Push(TaskRef task) {
		WRITE_LOCK;
		_tasks.push(task);
	}

	TaskRef Pop() {
		WRITE_LOCK;
		if (_tasks.empty()) {
			return nullptr;
		}

		TaskRef result = _tasks.front();
		_tasks.pop();
		return result;
	}

private:
	USE_LOCK;
	queue<TaskRef> _tasks;
};