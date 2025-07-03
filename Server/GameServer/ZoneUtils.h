#pragma once
#include "../ServerCore/Types.h"
#include "Vector2.h"
#include "Struct.pb.h"
#include "GameConst.h"
#include "MapDesc.h"

namespace ZoneUtils {
	inline uint64 MakeZoneCoordKey(int x, int y) {
		return (static_cast<uint64>(y) << 32) | static_cast<uint32>(x);
	}

	inline Vector2<int> GetZoneIndex(uint64 key) {
		int x = static_cast<int>(key & 0xFFFFFFFF);          // 하위 32비트
		int y = static_cast<int>((key >> 32) & 0xFFFFFFFF);  // 상위 32비트
		return Vector2<int>(x, y);
	}

	inline Vector2<int> GetZoneIndex(const MapDesc& mapDesc, const Protocol::LocationYaw& pos) {
		return Vector2<int>(
			static_cast<int>(pos.x() / mapDesc.zoneWidth),
			static_cast<int>(pos.y() / mapDesc.zoneHeight)
		);
	}

	/*inline Vector2<int> GetGridCoord(const Protocol::LocationYaw& pos) {
		const int gridX = static_cast<int>(pos.x()) % GameConst::ZONE_WIDTH / GameConst::GRID_WIDTH;
		const int gridY = static_cast<int>(pos.y()) % GameConst::ZONE_HEIGHT / GameConst::GRID_HEIGHT;
		return Vector2<int>(gridX, gridY);
	}*/
}