#include "pch.h"
#include "ObjectUtils.h"
#include "Player.h"
#include "ClientSession.h"

atomic<uint64> ObjectUtils::_sIdGenerator = 1;

PlayerRef ObjectUtils::CreatePlayer(ClientSessionRef session)
{
    const uint64 newId = _sIdGenerator.fetch_add(1);

    PlayerRef player = ObjectPool<Player>::MakeShared();
    player->SetOwnerSession(session);
    
    session->SetCurPlayer(player);
    
    Protocol::PlayerInfo& playerInfo = player->GetPlayerInfo();
    playerInfo.set_id(newId);

    return player;
}
