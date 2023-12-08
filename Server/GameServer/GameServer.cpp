#include "pch.h"
#include <iostream>
#include "CorePch.h"
#include <thread>
#include <atomic>
#include <mutex>
#include <Windows.h>
#include <future>

using namespace std;

//__declspec(thread) int32 value;
thread_local int32 LThreadId = 0;

//tls(thread local storage)
//->스레드 별로 고유한 저장 공간을 뜻함.
//
//tls, stack 차이
//->각각의 스레드는 고유한 스택을 갖기 떄문에 스레드 별로 고유함.
//->ex) 각각의 스레드가 같은 함수를 실행한다고 해도 그 함수에서 정의된 지역 변수는
//->실제로 다른 메모리 공간에 위치한다는 의미다.
//->그러나 정적 변수나 전역 변수의 경우에는 프로세스 내의 모든 스레드에 의해서 공유됩니다.
//
//1. tls 의 경우 정적, 전역 변수를 각각의 스레드에게 독립적으로 만들어 주고 싶을 떄 사용하는 것이다.
//다시 말해, 같은 문장을 실행하고 있지만 실제로는 스레드 별로 다른 주소 공간을 상대로 작업하는 것이다.

void ThreadMain(int32 threadId) {
	LThreadId = threadId;

	while (true) {
		cout << "Hi! I am Thread " << LThreadId << endl;

		Sleep(1000);
	}
}

int main() {
	vector<thread> threads;

	for (int32 i = 0; i < 10; i++) {
		int threadId = i + 1;
		threads.push_back(thread(ThreadMain, threadId));
	}

	for (thread& t : threads) {
		t.join();
	}
}
	