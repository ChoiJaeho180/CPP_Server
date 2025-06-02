#include "pch.h"
#include "ReceiveBuffer.h"

ReceiveBuffer::ReceiveBuffer(int32 bufferSize)
{
	_bufferSize = bufferSize;
	_capacity = _bufferSize * BUFFER_COUNT;
	_buffer.resize(_capacity);
}

ReceiveBuffer::~ReceiveBuffer()
{

}

void ReceiveBuffer::Clean()
{
	int dataSize = DataSize();
	if (dataSize == 0) {
		// buffer에 있는 모든 데이터를 읽었기 때문에 초기화
		_readPos = 0;
		_writePos = 0;
	}
	else {
		// 여유 공간이 _bufferSize 보다 작으면 앞으로 당기는 작업 처리
		if (_bufferSize > FreeSize()) {
			::memcpy(&_buffer[0], &_buffer[_readPos], dataSize);
			_readPos = 0;
			_writePos = dataSize;
		}
	}
}

bool ReceiveBuffer::OnRead(int32 numOfBytes)
{
	if (numOfBytes > DataSize()) {
		return false;
	}

	_readPos += numOfBytes;
	return true;
}

bool ReceiveBuffer::OnWrite(int32 numOfBytes)
{
	if (numOfBytes > FreeSize()) {
		return false;
	}

	_writePos += numOfBytes;
	return true;
}
