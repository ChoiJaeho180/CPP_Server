#pragma once
#include "Types.h"

class IEntity
{
	virtual uint64 GetId() const = 0;
	virtual Protocol::ObjectType GetType() const = 0;
	virtual FVector GetPosition() const = 0;
	virtual void SetPosition(const FVector& pos) = 0;
	virtual float GetYaw() const = 0;
	virtual void SetYaw(float yaw) = 0;
};