#pragma once


class ObjectUtils
{

public:
	// LobbyPlayerInfo �ӽ� ���� ���� ����
	static PlayerRef CreatePlayer(const Protocol::LobbyPlayerInfo& info);
private:
	static atomic<uint64> _sIdGenerator;
};

