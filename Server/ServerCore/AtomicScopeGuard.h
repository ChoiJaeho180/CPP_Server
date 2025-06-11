template<typename T>
class AtomicScopeGuard 
{
public:
	AtomicScopeGuard(std::atomic<T>& target, T expected, T desired, T restoreValue = expected)
		: _target(target), _restoreValue(restoreValue), _acquired(false)
	{
		T exp = expected;
		_acquired = _target.compare_exchange_strong(exp, desired);
	}

	~AtomicScopeGuard() {
		if (_acquired) {
			_target.store(_restoreValue);
		}
	}

	// if (guard) 사용 가능
	explicit operator bool() const {
		return _acquired;
	}

private:
	std::atomic<T>& _target;
	T _restoreValue;
	bool _acquired;
};