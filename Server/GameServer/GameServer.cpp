#include "pch.h"
#include <iostream>
#include "CorePch.h"
#include <thread>
#include <atomic>
#include <mutex>
#include <Windows.h>
#include <future>

using namespace std;

// 문제 1. 가시성, 2. 코드 재배치.
// 1. 가시성 : 
// -> 각각의 코어는 메인 메모리와 별도로 각각의 레지스터와 캐쉬를 가지고 있다.
// -> 컴파일러는 최적화 때문에 코어의 레지스터와 캐쉬를 이용한다.
// 결론
// -> 멀티 쓰레딩 환경에서는, 한 코어에서 값 변경을 여러 코어가 공유하도록,
// 메인 메모리로 반영하는 작업을 적절히 수행해야 메모리 가시성을 유지할 수 있다.

// 2. 코드 재배치.
// -> 결과가 동일하다면, 컴파일러나 cpu가 코드 순서를 바꿔서 최적화를 할 수 있다.
// -> 멀티 쓰레드 환경을 고려하지 않고 단일 쓰레드를 기준으로 고려하여 코드 순서를 바꾼다.

// 

int x;
int y;
int r1;
int r2;

// 컴파일러가 최적화 하지 말라는 키워드 volatile
volatile bool ready;

void Thread_1() {

	while (!ready)
		;

	y = 1;
	r1 = x;
}

void Thread_2() {
	while (!ready)
		;

	x = 1;
	r2 = y;
}

int main() {
	int count = 0;
	while (true) {
		ready = false;
		count++;

		x = y = r1 = r2 = 0;

		thread t1(Thread_1);
		thread t2(Thread_2);

		ready = true;

		t1.join();
		t2.join();

		if (r1 == 0 && r2 == 0) {
			break;
		}
	}

	cout << count << "번 만에 빠져나옴~" << endl;



}