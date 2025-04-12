#pragma once
class ReceiveBuffer
{
	// 최적화
	// readPos와 writePos가 동일한 경우 메모리 복사 없이 초기화 가능하므로
	// bufferSize보다 10배 크게 잡아 read/write 포인터가 자주 겹치도록 하여,
	// memcpy 호출 빈도를 줄여 성능을 최적화함
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

