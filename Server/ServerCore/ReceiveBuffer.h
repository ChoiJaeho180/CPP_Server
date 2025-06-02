#pragma once

// 동작
// 1. 버퍼에 데이터를 수신하면 writePos 증가
// 2. 버퍼에서 데이터를 처리하면 readPos 증가
// 3. writePos, readPos 같다면 버퍼의 데이터를 모두 처리했기 때문에, writePos, readPos 값 0으로 초기화
// 4. FreeSize()가 bufferSize보다 작으면(즉, 쓰기 여유 공간이 부족하면), 
//    아직 읽지 않은 데이터를 버퍼 앞쪽으로 memcpy하여 공간을 확보한다.

// 최적화
// readPos와 writePos가 동일한 경우, memcpy 없이 포인터만 초기화할 수 있다.
// 따라서 버퍼 크기를 bufferSize의 10배로 크게 설정해,
// readPos와 writePos가 자주 겹치도록 유도함으로써 memcpy 발생 빈도를 줄인다.

class ReceiveBuffer
{
	// 최적화
	enum { BUFFER_COUNT = 10 };
public:
	ReceiveBuffer(int32 bufferSize);
	~ReceiveBuffer();

	void			Clean();
	bool			OnRead(int32 numOfBytes);
	bool			OnWrite(int32 numOfBytes);

	BYTE*			ReadPos() { return &_buffer[_readPos]; }
	BYTE*			WritePos() { return &_buffer[_writePos]; }

	int32			DataSize() { return _writePos - _readPos; }
	int32			FreeSize() { return _capacity - _writePos; }

private:
	int32			_capacity = 0;
	int32			_bufferSize = 0;
	int32			_readPos = 0;
	int32			_writePos = 0;
	vector<BYTE>	_buffer;
};

