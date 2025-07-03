#pragma once
#include "GenProcedures.h"

namespace Protocol { class ObjectInfo; }
class DBToProtoMapper
{
public:
	static void ToObjectInfo(SP::GetOrCreatePlayer& sp, OUT Protocol::ObjectInfo& out);
	
};

