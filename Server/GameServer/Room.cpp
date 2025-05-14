#include "pch.h"
#include "Room.h"
#include "Player.h"
#include "ClientPacketHandler.h"
#include "ClientSession.h"

Room GRoom;

void Room::EnterRoom(PlayerRef player)
{
    WRITE_LOCK;
    
    uint64 playerId = player->PlayerId();
    ASSERT_CRASH(_players.find(playerId) == _players.end());

    _players.insert(pair<uint64, PlayerRef>(playerId, player));
    for (auto& player : _players) {
        cout << "playerId : " << player.second.get()->PlayerId() << endl;
    }
}

void Room::LeaveRoom(PlayerRef player)
{
    WRITE_LOCK;

    uint64 playerId = player->PlayerId();
    ASSERT_CRASH(_players.find(playerId) != _players.end());

    _players.erase(playerId);

}

void Room::BroadCast(SendBufferRef sendBuffer)
{
    WRITE_LOCK;
    for (auto& player : _players) {
        if (auto locked = player.second.get()->ownerSession().lock()) {
            locked->Send(sendBuffer);
        }
    }
}

