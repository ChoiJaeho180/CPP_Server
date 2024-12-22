#pragma once


/* ------------------
* RefCountable
 ------------------*/
class RefCountable
{
public:
	RefCountable(): _refCount(1) {}
	virtual ~RefCountable(){}

	int32 GetRefCount() { return _refCount; }
	int32 AddRef() { return ++_refCount; }
	int32 ReleaseRef() {
		int32 refCount = --_refCount;
		if (refCount == 0) {
			delete this;
		}
		return refCount;
	}
protected:
	atomic<int32> _refCount;
};

/* ------------------
* SharedPtr : _ptr의 레퍼런스 카운팅을 관리하는 객체
 ------------------*/
template<typename T>
class TSharedPtr {
public:
	TSharedPtr() {}
	TSharedPtr(T* ptr) { Set(ptr); }

	// 복사
	TSharedPtr(const TSharedPtr& rhs) { Set(rhs); }
	// 이동
	TSharedPtr(TSharedPtr&& rhs) { _ptr = rhs._ptr; rhs._ptr = nullptr; }
	// 상속 관계 복사
	template<typename U>
	TSharedPtr(const TSharedPtr<U>& rhs) { Set(static_cast<T*>(rhs._ptr)); }

	~TSharedPtr() { Release(); }
public:
	// 복사 연산자
	TSharedPtr& operator=(const TSharedPtr& rhs) {
		if (_ptr != rhs._ptr) {
			Release();
			Set(rhs._ptr);
		}
		return *this;
	}

	TSharedPtr& operator=(TSharedPtr&& rhs) {
		Release();
		_ptr = rhs._ptr;
		rhs._ptr = nullptr;
		return *this;
	}

	bool			operator==(const TSharedPtr& rhs) { return _ptr == rhs._ptr; }
	bool			operator==(T* ptr) { return _ptr == ptr; }
	bool			operator!=(const TSharedPtr& rhs) { return _ptr != rhs._ptr; }
	bool			operator!=(T* ptr) { return _ptr != ptr; }
	bool			operator<(const TSharedPtr& rhs) { return _ptr < rhs._ptr; }
	T*				operator*() { return _ptr; }
	const T*		operator*() const { return _ptr; }
					operator T*() const { return _ptr; }
	T*				operator->() { return _ptr; }
	const T*		operator->() const { return _ptr; }

	bool isNull() { return this->_ptr == nullptr; }
private:
	inline void Set(T* ptr) {
		_ptr = ptr;
		if (_ptr) {
			_ptr->AddRef();
		}
	}

	inline void Release() {
		if (_ptr != nullptr) {
			_ptr->ReleaseRef();
			_ptr = nullptr;
		}
	}

private:
	T* _ptr = nullptr;
};
