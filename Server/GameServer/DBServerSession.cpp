#include "pch.h"
#include "DBServerSession.h"
#include "DBWorkerManager.h"
#include "DBServerPacketHandler.h"

DBServerSessionRef DBServerSession::_instance = nullptr;

void DBServerSession::OnConnected()
{
	
	DBServerSession::SetInstance(static_pointer_cast<DBServerSession>(shared_from_this()));

}

void DBServerSession::OnDisconnected()
{
	_instance = nullptr;
}

void DBServerSession::OnRecvPacket(BYTE* buffer, int32 len)
{
    PacketSessionRef session = GetPacketSessionRef();
    DBPacketRef packet = DBServerPacketHandler::ParsePacket(buffer, len);
    if (packet == nullptr) {
        return;
    }

    cout << "DBServerSession::OnRecvPacket!!!" << endl;
    GDBServerCallbackMgr->EnqueuePacket(packet);
}

void DBServerSession::OnSend(int32 len)
{

}