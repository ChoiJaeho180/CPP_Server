#pragma once
#include <mutex>

class Account {

};

class AccountManager
{
public:
	static AccountManager* Instance() {
		static AccountManager instance;
		return &instance;
	}

	Account* GetAccount(int32 id) {
		lock_guard<mutex> lg(_mutex);
		return nullptr;
	}

	void ProcessLogin();
private:
	mutex _mutex;
};

