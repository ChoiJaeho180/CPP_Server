#pragma once


class ObjectUtils
{

public:
	// LobbyPlayerInfo 임시 추후 수정 예정
	static PlayerRef CreatePlayer(const Protocol::LobbyPlayerInfo& info);
private:
	static atomic<uint64> _sIdGenerator;
};

