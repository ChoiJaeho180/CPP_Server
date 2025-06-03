#pragma once


class ObjectUtils
{

public:
	static PlayerRef CreatePlayer(ClientSessionRef session);
private:
	static atomic<uint64> _sIdGenerator;
};

