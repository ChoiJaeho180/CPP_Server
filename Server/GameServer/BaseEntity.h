#pragma once
#include "Struct.pb.h"
#include "Protocol.pb.h"

class BaseEntity 
{
public:
	virtual ~BaseEntity() {}
	Protocol::ObjectInfo&			GetObjectInfo() { return _objectInfo; }
	Protocol::LocationYaw*			GetLocationYaw() { return _objectInfo.mutable_locationyaw(); }
	void							SetObjectInfo(const Protocol::ObjectInfo& objectInfo) { _objectInfo.CopyFrom(objectInfo); }
	void							SetObjectInfo(const Protocol::ObjectInfo* objectInfo) { _objectInfo.CopyFrom(*objectInfo); }
protected:
	Protocol::ObjectInfo			_objectInfo;
};