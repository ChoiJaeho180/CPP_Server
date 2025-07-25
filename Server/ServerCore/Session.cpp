#include "pch.h"
#include "Session.h"
#include "SocketUtils.h"
#include "Service.h"

Atomic<uint64> Session::GSessionIdGenerator = 1;

Session::Session(): _recvBuffer(BUFFER_SIZE)
{
    _socket = SocketUtils::CreateSocket();
    _sessionId = RoutingKey::MakeSessionKey(GSessionIdGenerator.fetch_add(1));
}

Session::~Session()
{
    SocketUtils::Close(_socket);
}

void Session::Send(SendBufferRef sendBufferRef)
{
    
    if (IsConnected() == false) {
        return;
    }

    bool registerSend = false;
    {
        WRITE_LOCK;
        _sendQueue.push(sendBufferRef);
        cout << "sendQueue push : " << _sendQueue.size() << endl;
            
        // 현재 RegisterSend가 걸리지 않은 상태라면, 걸어준다.
        if (_sendRegistered.exchange(true) == false) {
            registerSend = true;
        }
    }
    
    if (registerSend == true) {
        RegisterSend();
    }
}

bool Session::Connect()
{
    return RegisterConnect();
}

void Session::Disconnect(const WCHAR* cause)
{
    if (_connected.exchange(false) == false) {
        return;
    }

    // TEMP
    wcout << "Disconnect : " << cause << endl;
    
    RegisterDisconnect();
}

HANDLE Session::GetHandle()
{
    return reinterpret_cast<HANDLE>(_socket);
}

void Session::Dispatch(IocpEvent* iocpEvent, int32 numOfBytes)
{
    switch (iocpEvent->eventType) {
    case EventType::Connect:
        ProcessConnect();
        break;
    case EventType::Disconnect: 
        ProcessDisconnect();
        break;
    case EventType::Recv:
        ProcessRecv(numOfBytes);
        break;
    case EventType::Send:
        ProcessSend(numOfBytes);
        break;
    default:
        break;
    }
}

bool Session::RegisterConnect()
{
    if (IsConnected()) {
        return false;
    }

    if (GetService()->GetServiceType() != ServiceType::Client) {
        return false;
    }

    if (SocketUtils::SetReuseAddress(_socket, true) == false) {
        return false;
    }

    if (SocketUtils::BindAnyAddress(_socket, 0/*남는거*/) == false) {
        return false;
    }

    _connectEvent.Init();
    _connectEvent.owner = shared_from_this();

    DWORD numOfBytes = 0;
    SOCKADDR_IN sockAddr = GetService()->GetNetAddress().GetSockAddr();
    
    if (false == SocketUtils::ConnectEx(_socket, reinterpret_cast<SOCKADDR*>(&sockAddr), sizeof(sockAddr), nullptr, 0, &numOfBytes, &_connectEvent)) {
        int32 errorCode = ::WSAGetLastError();
        if (errorCode != WSA_IO_PENDING) {
            _connectEvent.owner = nullptr;
            return false;
        }
    }

    return true;
}

bool Session::RegisterDisconnect()
{
    _disconnectEvent.Init();
    _disconnectEvent.owner = shared_from_this();

    if (false == SocketUtils::DisconnectEx(_socket, &_disconnectEvent, TF_REUSE_SOCKET, 0)) {
        int32 errorCode = ::WSAGetLastError();
        if (errorCode != WSA_IO_PENDING) {
            _disconnectEvent.owner = nullptr;
            return false;
        }
    }

    return true;
}

void Session::RegisterRecv()
{
    if (IsConnected() == false) {
        return;
    }

    _recvEvent.Init();
    _recvEvent.owner = shared_from_this();

    WSABUF wsaBuf;
    wsaBuf.buf = reinterpret_cast<char*>(_recvBuffer.WritePos());
    wsaBuf.len = _recvBuffer.FreeSize();
    DWORD numOfBytes = 0;
    DWORD flags = 0; 
    if (SOCKET_ERROR == ::WSARecv(_socket, &wsaBuf, 1, OUT &numOfBytes, OUT &flags, &_recvEvent, nullptr)) {
        int32 errorCode = ::WSAGetLastError();
        if (errorCode != WSA_IO_PENDING) {
            HandleError(errorCode);
            _recvEvent.owner = nullptr; // RELEASE_REF;
        }
    }
}

void Session::RegisterSend()
{
    if (IsConnected() == false) {
        return;
    }

    _sendEvent.Init();
    _sendEvent.owner = shared_from_this();
    
    {
        WRITE_LOCK;
        
        int32 writeSize = 0;
        while (_sendQueue.empty() == false) {
            SendBufferRef sendBuffer = _sendQueue.front();
            writeSize += sendBuffer->WriteSize();
            // todo. 일정량 이상 보낼 경우 끊어주는 처리 필요.

            _sendQueue.pop();
            _sendEvent.sendBuffers.push_back(sendBuffer);
        }
    }

    // Scatter-Gather : 흩어져 있는 데이터들을 모아서 한번에 보낸다.
    Vector<WSABUF> wsaBufs;
    wsaBufs.reserve(_sendEvent.sendBuffers.size());
    for (SendBufferRef sendBuffer : _sendEvent.sendBuffers) {
        WSABUF wsaBuf;
        wsaBuf.buf = reinterpret_cast<char*>(sendBuffer->Buffer());
        wsaBuf.len = static_cast<ULONG>(sendBuffer->WriteSize());
        wsaBufs.push_back(wsaBuf);
    }

    DWORD numOfBytes = 0;
    if (SOCKET_ERROR == ::WSASend(_socket, wsaBufs.data(), static_cast<DWORD>(wsaBufs.size()), OUT & numOfBytes, 0, &_sendEvent, nullptr)) {
        int32 errorCode = ::WSAGetLastError();
        if (errorCode != WSA_IO_PENDING) {
            HandleError(errorCode);
            // release_ref
            _sendEvent.owner = nullptr;
            _sendEvent.sendBuffers.clear();
            _sendRegistered.store(false);
        }
    }
}

void Session::ProcessConnect()
{
    _connectEvent.owner = nullptr;

    _connected.store(true);
    GetService()->AddSession(GetSessionRef());

    // 컨텐츠 코드에서 재정의
    OnConnected();

    // 수신 등록
    RegisterRecv();
}

void Session::ProcessDisconnect()
{
    _disconnectEvent.owner = nullptr;
    OnDisconnected();
    GetService()->ReleaseSession(GetSessionRef());
}

void Session::ProcessRecv(int32 numOfBytes)
{
    _recvEvent.owner = nullptr; // RELEASE_REF;

    // 0을 받았다는건 연결이 끊겼다는 의미.
    if (numOfBytes == 0) {
        Disconnect(L"Recv 0");
        return;
    }

    if (_recvBuffer.OnWrite(numOfBytes) == false) {
        Disconnect(L"OnWrite Overflow");
        return;
    }
    
    int32 dataSize = _recvBuffer.DataSize();
    int processLen = OnRecv(_recvBuffer.ReadPos(), dataSize);
    if (0 > processLen || processLen > dataSize || _recvBuffer.OnRead(processLen) == false) {
        Disconnect(L"OnRead Overflow");
        return;
    }

    _recvBuffer.Clean();

    // 수신 등록
    RegisterRecv();
}

void Session::ProcessSend(int32 numOfBytes)
{
    // release_ref
    _sendEvent.owner = nullptr;
    _sendEvent.sendBuffers.clear(); 
    

    if (numOfBytes == 0) {
        Disconnect(L"Send 0");
    }

    // 컨텐츠 코드에서 재정의
    OnSend(numOfBytes);

    bool registerSend;
    {
        WRITE_LOCK;
        if (_sendQueue.empty()) {
            _sendRegistered.store(false);
            registerSend = false;
        }
        else {
            registerSend = true;
        }
    }

    if (registerSend == true) {
        RegisterSend();
    }
}

void Session::HandleError(int32 errorCode)
{
    switch (errorCode) {
    case WSAECONNRESET:
    case WSAECONNABORTED:
        Disconnect(L"HandleError");
        break;
    default:
         // TODO : Log
         cout << "Handle Error : " << errorCode << endl;
         break;
    }
}

PacketSession::PacketSession()
{
}

PacketSession::~PacketSession()
{
}

int32 PacketSession::OnRecv(BYTE* buffer, int32 len) {
    int32 processLen = 0;
    while (true) {
        int32 dataSize = len - processLen;

        // 최소한 헤더는 파싱할 수 있어야 한다.
        if (dataSize < sizeof(PacketHeader)) {
            break;
        }

        PacketHeader header = *(reinterpret_cast<PacketHeader*>(&buffer[processLen]));
        // 헤더에 기록된 패킷 크기를 파싱할 수 있어야 한다.
        if (dataSize < header.size) {
            break;
        }

        // 패킷 하나가 온전히 왔다는걸 신뢰
        OnRecvPacket(&buffer[processLen], header.size);

        processLen += header.size;
    }

    return processLen;
}
