#include "pch.h"
#include "Room.h"
#include "Player.h"
#include "ClientPacketHandler.h"
#include "ClientSession.h"

shared_ptr<Room> GRoom = MakeShared<Room>();

void Room::Enter(PlayerRef player)
{
    uint64 playerId = player->PlayerId();
    ASSERT_CRASH(_players.find(playerId) == _players.end());

    _players.insert(pair<uint64, PlayerRef>(playerId, player));
    for (auto& player : _players) {
        cout << "playerId : " << player.second.get()->PlayerId() << endl;
    }
}

void Room::Leave(PlayerRef player)
{
    uint64 playerId = player->PlayerId();
    ASSERT_CRASH(_players.find(playerId) != _players.end());

    _players.erase(playerId);
}

void Room::BroadCast(SendBufferRef sendBuffer)
{
    for (auto& player : _players) {
        if (auto locked = player.second.get()->ownerSession().lock()) {
            locked->Send(sendBuffer);
        }
    }
}


