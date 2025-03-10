#pragma once

/*--------------
    IocpObject
---------------*/
class IocpObject {
public:
    virtual HANDLE GetHandle() abstract;
    virtual void Dispatch(class IocpEvent* iocpEvent, int32 numOfBytes = 0) abstract;
};

/*--------------
    IocpCore
---------------*/

class IocpCore
{
public:
    IocpCore();
    ~IocpCore();

    HANDLE GetHandle() { return _iocpHandle; }

    bool Register(class IocpObject* iocpObject);

    // 워커 스레드들이 Iocp의 일감을 관찰하는 함수
    bool Dispatch(int32 timeOutMs = INFINITE);
private:
    HANDLE _iocpHandle;
};

extern IocpCore GIocpCore;