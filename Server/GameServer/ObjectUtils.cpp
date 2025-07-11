#include "pch.h"
#include "ObjectUtils.h"
#include "Player.h"
#include "ClientSession.h"

atomic<uint64> ObjectUtils::_sIdGenerator = 1;

PlayerRef ObjectUtils::CreatePlayer(const Protocol::LobbyPlayerInfo& info)
{
    const uint64 newId = _sIdGenerator.fetch_add(1);

    PlayerRef player = ObjectPool<Player>::MakeShared();
    
    Protocol::ObjectInfo& playerInfo = player->GetObjectInfo();
    playerInfo.set_id(newId);
    playerInfo.set_level(info.level());
    playerInfo.set_exp(info.exp());
    playerInfo.set_mapcmsid(info.mapid());
    
    Protocol::LocationYaw* location = player->GetLocationYaw();
    location->set_id(newId);
    location->set_x(info.x());
    location->set_y(info.y());
   

    return player;
}
