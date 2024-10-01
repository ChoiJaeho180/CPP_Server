#include "pch.h"
#include <iostream>
#include "CorePch.h"
#include "AccountManager.h"
#include "UserManager.h"
#include <windows.h>
#include <atomic>
#include "ThreadManager.h"
#include "GameServer.h"

class TestLock {
	USE_LOCK;
public:
	int32 TestRead() {
		READ_LOCK;

		if (this->_queue.empty()) {
			return -1;
		}

		return this->_queue.front();
	}

	void TestPush() {
		WRITE_LOCK;

		this->_queue.push(rand() % 100);
	}

	void TestPop() {
		WRITE_LOCK;
		while (true) {

		}
		if (this->_queue.empty() == false) {
			this->_queue.pop();
		}
	}
private:
	queue<int32> _queue;
};

TestLock testLock;


void ThreadWrite() {
	while (true) {
		testLock.TestPush();
		this_thread::sleep_for(1ms);
		testLock.TestPop();
	}
}
void ThreadRead() {
	while (true) {
		int32 value = testLock.TestRead();
		cout << value << endl;
		this_thread::sleep_for(1ms);
	}
}



int main() {

	for (int32 i = 0; i <2; i++) {
		GThreadManager->Launch(ThreadWrite);
	}

	for (int32 i = 0; i < 3; i++) {
		GThreadManager->Launch(ThreadRead);

	}
	GThreadManager->Join();

}
