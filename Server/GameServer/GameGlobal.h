#pragma once

extern class DBResponseManager* GDBServerCallbackMgr;

class GameGlobal
{
public:
	static void Init();
	static void Destory();
	;
private:
	static bool _initialized;
};