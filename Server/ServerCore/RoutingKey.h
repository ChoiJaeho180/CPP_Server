#pragma once
#include "Types.h"

namespace RoutingKey
{
	constexpr uint64 MakeSessionKey(uint64 sessionId) { return sessionId | (1ULL << 63); }
	constexpr uint64 MakePlayerKey(uint64 playerId) { return playerId & ~(1ULL << 63); }

	constexpr bool IsSessionKey(uint64 key) { return (key & (1ULL << 63)) != 0; }
	constexpr uint64 ExtractOriginalId(uint64 key) { return key & ~(1ULL << 63); }
}