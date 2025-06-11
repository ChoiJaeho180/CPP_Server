#pragma once

class BaseEntity 
{
public:
	virtual ~BaseEntity() {}
	Protocol::ObjectInfo& GetObjectInfo() { return _objectInfo; }
protected:
	Protocol::ObjectInfo _objectInfo;
};