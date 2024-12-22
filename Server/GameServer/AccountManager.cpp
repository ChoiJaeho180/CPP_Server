#include "pch.h"
#include "AccountManager.h"
#include "UserManager.h"
#include "PlayerManager.h"

AccountManager GAccountManager;

void AccountManager::AccountThenPlayer()
{
	WRITE_LOCK;
	GPlayerManager.Lock();
}

void AccountManager::Lock()
{
	WRITE_LOCK;
}