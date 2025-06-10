#pragma once
#include "../ServerCore/Types.h"
#include "Struct.pb.h"

class BaseEntity 
{
public:
	virtual ~BaseEntity() {}

	virtual uint64 GetId() const { return _id; }
	virtual Protocol::ObjectType GetType() const { return _type; }

	virtual Protocol::LocationYaw GetLocationYaw() const { return _locationYaw; }
	virtual void SetLocationYaw(const Protocol::LocationYaw& locYaw) { _locationYaw = locYaw; }

protected:
	uint64 _id = 0;
	Protocol::ObjectType _type = Protocol::OBJECT_TYPE_NONE;
	Protocol::LocationYaw _locationYaw;
};