#include "pch.h"
#include "IocpCore.h"
#include "IocpEvent.h"

IocpCore::IocpCore()
{
	_iocpHandle = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	ASSERT_CRASH(_iocpHandle != INVALID_HANDLE_VALUE);
}

IocpCore::~IocpCore()
{
	::CloseHandle(_iocpHandle);
}

bool IocpCore::Register(IocpObjectRef iocpObject)
{
	HANDLE handle = ::CreateIoCompletionPort(iocpObject->GetHandle(),_iocpHandle,0, 0);
	if (handle == NULL) {
		DWORD error = GetLastError();
		std::cout << "CreateIoCompletionPort failed: " << error << std::endl;
	}
	return handle;
}

bool IocpCore::Dispatch(int32 timeoutMs)
{
	DWORD numOfBytes = 0;
	ULONG_PTR key = 0;
	IocpEvent* iocpEvent = nullptr;
	if (::GetQueuedCompletionStatus(_iocpHandle, OUT & numOfBytes, OUT &key,OUT reinterpret_cast<LPOVERLAPPED*>(&iocpEvent), timeoutMs)) {
		IocpObjectRef iocpObject = iocpEvent->owner;
		iocpObject->Dispatch(iocpEvent, numOfBytes);
	}
	else {
		int32 errCode = ::WSAGetLastError();
		switch (errCode)
		{
		case WAIT_TIMEOUT:
			return false;
		default:
			// TODO ·Î±× Âï±â
			IocpObjectRef iocpObject = iocpEvent->owner;
			iocpObject->Dispatch(iocpEvent, numOfBytes);
			break;
		}
	}

	return false;
}
