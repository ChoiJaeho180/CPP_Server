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

	// 1. �� ��带 �����.
	// 2. �� ����� next�� ���� ���� �����Ѵ�.
	// 3. ���� ��带 �� ���� �Ҵ��Ѵ�. (�� ���ܿ��� ���� ������ ����.)
	void Push(const T& value) {

		Node* newNode = new Node(value);
		newNode->next = _head;
		
		// compare_exchange_weak : ���������� �Ʒ��� ������ ���������� ������
		/*if (_head == newNode->next) {
			_head = newNode;
			return true;
		}
		else {
			newNode->next = _head;
			return false;
		}*/


		// ���� �����尡 
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

		// �޸� ������ �� �޸� �� ��
		// exception�� �Ű澲�� �ʴ´�.
		value = oldHead->data;

		TryDelete(oldHead);
		return true;
	}

	// 1. ������ �и�
	// 2. Count üũ
	// 3. �� ȥ�ڸ� ����.
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