#pragma once

namespace GameConst {

	inline constexpr int GAME_WORKER_COUNT = 2;
	inline constexpr int DB_WORKER_COUNT = 2;
	inline constexpr int DB_JOB_SHARD_COUNT = DB_WORKER_COUNT;
}