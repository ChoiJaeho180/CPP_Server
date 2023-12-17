#pragma once
#include<mutex>

template<typename T>
class LockStack {
private:
	stack<T> _stack;
	mutex _mutex;
	condition_variable _condVar;
public:
	LockStack() {}
	LockStack(const LockStack&) = delete;
	LockStack& operator=(const LockStack&) = delete;

	void Push(T value) {
		lock_guard<mutex> m(_mutex);
		_stack.push(std::move(value));

		_condVar.notify_one();
	}

	bool TryPop(T& value) {

		lock_guard<mutex> m(_mutex);
		if (_stack.empty()) {
			return false;
		}

		value = std::move(_stack.top());
		_stack.pop();

		return true;
;	}

	void WaitPop(T& value) {
		unique_lock<mutex> m(_mutex);
		_condVar.wait(m, [this] {_stack.empty() == false; });

		value = std::move(_stack.top());
		_stack.pop();
	}
};


template<typename T> 
class LockFreeStack {
private:
	struct Node {
		Node* next;
		T data;

		Node(const T& value) : data(value) {

		}
	};
	atomic<Node*> _head;
	atomic<uint32> _popCount = 0;
	atomic<Node*> _pendingList;
public:

	// 1. 새 노드를 만든다.
	// 2. 새 노드의 next를 현재 헤드로 설정한다.
	// 3. 현재 헤드를 새 노드로 할당한다. (이 스텝에서 경합 문제가 생김.)
	void Push(const T& value) {

		Node* newNode = new Node(value);
		newNode->next = _head;
		
		// compare_exchange_weak : 내부적으로 아래의 스텝이 원자적으로 동작함
		/*if (_head == newNode->next) {
			_head = newNode;
			return true;
		}
		else {
			newNode->next = _head;
			return false;
		}*/


		// 여러 스레드가 
		while (_head.compare_exchange_weak(newNode->next, newNode) == false) {

		}
	}

	// 1. 
	bool TryPop(T& value) {

		++_popCount;

		Node* oldHead = _head;
		while (oldHead && _head.compare_exchange_weak(oldHead, oldHead->next) == false)
		{

		}

		if (oldHead == nullptr) {
			--_popCount;
			return false;
		}

		// 메모리 복사할 때 메모리 고갈 등
		// exception은 신경쓰지 않는다.
		value = oldHead->data;

		TryDelete(oldHead);
		return true;
	}

	// 1. 데이터 분리
	// 2. Count 체크
	// 3. 나 혼자면 삭제.
	void TryDelete(Node* oldHead) {
		if (_popCount == 1) {

			Node* node = _pendingList.exchange(nullptr);
			if (--_popCount == 0 ) {

				DeleteNodes(node);
			} 
			else if(node) {
				ChainPendingNodeList(node);
			}

			delete oldHead;
		}
		else {
			ChainPendingNode(oldHead);
			--_popCount;
		}
	}

	void ChainPendingNodeList(Node* first, Node* last) {
		last->next = _pendingList;
		while (_pendingList.compare_exchange_weak(last->next, first) == false) {

		}
	}

	void ChainPendingNodeList(Node* node) {
		Node* last = node;
		while (last->next) {
			last = last->next;
		}
		ChainPendingNodeList(node, last);
	}

	void ChainPendingNode(Node* node) {
		ChainPendingNodeList(node, node);
	}

	static void DeleteNodes(Node* node) {
		cout << node << endl;
		while (node) {
			Node* nextNode = node->next;
			delete node;
			node = nextNode;
		}
	}
};