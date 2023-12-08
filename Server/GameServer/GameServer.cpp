#include "pch.h"
#include <iostream>
#include "CorePch.h"
#include <thread>
#include <atomic>
#include <mutex>
#include <Windows.h>
#include <future>

using namespace std;

atomic<bool> flag;


int main() {

	flag = false;
	//flag = true;
	flag.store(true, memory_order::memory_order_seq_cst);

	//bool val = flag;
	bool val = flag.load(memory_order::memory_order_seq_cst);

	// 이전 flag 값을 prev에 넣고, flag 값을 수정.
	{
		// bool prev = flag;
		// flag = true;

		bool prev = flag.exchange(true);
	}

	// CAS (Compare-And-Swap) 조건부 수정
	{
		bool expected = false; // 예상 값
		bool desired = true; // 변경 값.
		flag.compare_exchange_strong(expected, desired);

		if (flag == expected) {
			expected = flag;
			flag = desired;
			return true;
		}
		else {
			expected = flag;
			return false;
		}
	}
}