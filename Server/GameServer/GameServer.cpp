#include "pch.h"
#include <iostream>
#include "CorePch.h"
#include <thread>
#include <atomic>
#include <mutex>
#include <Windows.h>
#include <future>
#include "ConcurrentQueue.h"
#include "ConcurrentStack.h"

using namespace std;

//LockQueue<int32> q;
LockFreeStack<int32> s;

void push() {
	while (true) {
		int32 value = rand() % 100;
		s.Push(value);

		this_thread::sleep_for(10ms);
	}
}

void pop() {
	while (true) {
		 int32 value;
		 if (s.TryPop(OUT value)) {
			 cout << value << endl;

		 }
	}
}


int main() {
	thread t1(push);
	thread t2(pop);
	//thread t3(pop);

	t1.join();
	t2.join();
	//t3.join();
}
	