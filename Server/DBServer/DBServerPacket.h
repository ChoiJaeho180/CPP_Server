#pragma once
#include "DBProtocol.pb.h"
#include "../ServerCore/Session.h"
#include <optional>

struct DBServerPacket {
	ServerPacketHeader header;
	std::shared_ptr<google::protobuf::Message> data;
};

using DBServerPacketRef = std::shared_ptr<DBServerPacket>;
