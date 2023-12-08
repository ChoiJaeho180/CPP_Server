#include "pch.h"
#include <iostream>
#include "CorePch.h"
#include <thread>
#include <atomic>
#include <mutex>
#include <Windows.h>
#include <future>

using namespace std;

atomic<bool> ready;
int32 value;

void Producer() {
	value = 10;

	ready.store(true, memory_order::memory_order_release);
}

void Consumer() {

	while (ready.load(memory_order::memory_order_acquire) == false)
		;

	cout << value << endl;
}

int main() {

	ready = false;
	value = 0;

	thread t1(Producer);
	thread t2(Consumer);
	t1.join();
	t2.join();

	// atomic Memory Model (정책)
	// 1) Sequentially Consistent (seq_cst)
	// 2) Acquire-Rleease (acquire, release, acq_rel)
	// 3) Relaxed (relaxed)

	// 1) seq_cst (가장 엄격 = 컴파일러 최적화 여지 적음 = 개발자 입장에서 직관적 = 코드 재배치가 보수적으로 일어남)
	// -> atomic default. 
	// -> 가시성 해결, 코드 재배치 문제 해결.
	// 
	// 2) acquire-release
	// -> release 명령 이전의 메모리 명령들이, 해당 명령 이후로 재배치 되는 것을 금지.
	// -> 그리고 acquire로 같은 변수를 읽는 쓰레드가 있다면
	// -> release 이전의 명령들이 -> acquire 하는 순간에 관찰 가능(가시성 보장)
	// 
	// 3) relaxed (자유롭다. = 컴파일러 최적화 여지 많음 = 개발자 입장에서 직관적이지 않음 = 코드 재배치가 빈번히 일어날 수 있음)
	// -> 가시성 해결 X, 코드 재배치 멋대로 가능
	// -> 가장 기본 조건 (동일 객체에 대한 동일 관전 순서만 보장)
}