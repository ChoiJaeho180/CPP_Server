#include "pch.h"
#include "GameGlobal.h"
DBResponseManager* GDBServerCallbackMgr = nullptr;
bool GameGlobal::_initialized = false;

void GameGlobal::Init() {
	if (_initialized) {
		return;
	}

	GDBServerCallbackMgr = new DBResponseManager;

}
void GameGlobal::Destory()
{
	if (!_initialized) {
		return;
	}

	delete GDBServerCallbackMgr;
}
