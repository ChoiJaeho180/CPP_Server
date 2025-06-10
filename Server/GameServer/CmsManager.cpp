#include "pch.h"
#include "CmsManager.h"
#include "../ServerCore/JsonUtils.h"
#include "ZoneDesc.h"
#include "ZoneadsadsaDesc.h"

void CmsManager::Init(const string& directory)
{
    CmsCached<ZoneDesc>::Load(directory + "/Zone.json");
    CmsCached<ZoneadsadsaDesc>::Load(directory + "/Zoneadsadsa.json");
}