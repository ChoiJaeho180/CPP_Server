#pragma once

// 구조 장점
// 1. 메모리 할당/해제에 따른 부하 제거
//		-> Chunk에서 메모리를 잘라 쓰기만 하므로, 할당 시간 일정
// 2. 메모리 단편화 방지
//		-> 크기가 다른 메모리를 반복해서 할당/해제하면,
//		-> 중간에 사용할 수 없는 조각난 메모리들이 생김 -> 단편화 발생
//		-> 조각난 메모리 때문에 실제 사용 가능한 메모리가 많아도,
//		-> 큰 연속 메모리를 확보하지 못해 할당 실패가 발생할 수 있음.
// 3. 락을 최소화하여 사용
//		-> thread_local을 사용했기 때문에 _sendBufferChunks에 Push, Pop
//		-> 할때를 제외하면 싱글스레드처럼 안전하게 사용 가능
class SendBufferChunk;

class SendBuffer 
{
public:
	SendBuffer(SendBufferChunkRef owner, BYTE* buffer,uint32 allocSize);
	~SendBuffer();

	BYTE*				Buffer() { return _buffer; }
	uint32				AllocSize() { return _allocSize; }
	uint32				WriteSize() { return _writeSize; }
	void				Close(uint32 writeSize);

private:
	BYTE*					_buffer;
	uint32					_allocSize = 0;
	uint32					_writeSize = 0;
	SendBufferChunkRef		_owner;
};

// SendBufferChunk : 
// 엄청 큰 메모리를 미리 할당하여 SendBuffer에 필요한 만큼 메모리를 반환
class SendBufferChunk : public enable_shared_from_this<SendBufferChunk> {
	enum {
		SEND_BUFFER_CHUNK_SIZE = 6000
	};
public:
	SendBufferChunk();
	~SendBufferChunk();

	void					Reset();
	SendBufferRef			Open(uint32 allocSize);
	void					Close(uint32 writeSize);
public:
	inline bool		    	IsOpen() { return _open; }
	inline uint32			FreeSize() { return static_cast<uint32>(_buffer.size() - _usedSize); }
	inline BYTE*			Buffer() { return &_buffer[_usedSize]; }
private:
	Array<BYTE, SEND_BUFFER_CHUNK_SIZE>		_buffer = {};
	bool									_open = false;
	uint32									_usedSize = 0;
};

class SendBufferManager {
public:
	SendBufferRef			Open(uint32 size);
private:
	SendBufferChunkRef		Pop();
	void					Push(SendBufferChunkRef buffer);

	static void				PushGlobal(SendBufferChunk* buffer);
private:
	USE_LOCK;
	Vector<SendBufferChunkRef> _sendBufferChunks;
};