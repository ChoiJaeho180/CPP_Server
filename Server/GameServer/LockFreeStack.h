#pragma once

// 1차 구현
/*template <typename T> 
struct Node {
	T date;
	Node* node;
};

struct SListEntry {
	SListEntry* next;
};

class Data {
public:
	SListEntry _entry;

	int _hp;
	int _mp;
};

struct SListHeader {
	SListEntry* next = nullptr;
};

void InitalizeHead(SListHeader* header);
void PushEntrySList(SListHeader* header, SListEntry* entry);
SListEntry* PopEntrySList(SListHeader* header)*/;


//template <typename T>
//struct Node {
//	T date;
//	Node* node;
//};
//
//struct SListEntry {
//	SListEntry* next;
//};
//
//class Data {
//public:
//	SListEntry _entry;
//
//	int _hp;
//	int _mp;
//};
//
//struct SListHeader {
//	SListEntry* next = nullptr;
//};
//
//void InitalizeHead(SListHeader* header);
//void PushEntrySList(SListHeader* header, SListEntry* entry);
//SListEntry* PopEntrySList(SListHeader* header);


template <typename T>
struct Node {
	T date;
	Node* node;
};

// 16byte 정렬 요청
DECLSPEC_ALIGN(16)
struct SListEntry {
	SListEntry* next;
};


struct SListHeader {
	SListHeader() {
		alignment = 0;
		region = 0;
	}
	union {
		struct {
			uint64  alignment;
			uint64 region;
		}DUMMYSTRUCTNAME;

		struct {
			uint64 depth : 16;
			uint64 sequence : 48;
			uint64 reserved : 4;
			uint64 next : 60;
		} HeaderX64;
	};
};

void InitalizeHead(SListHeader* header);
void PushEntrySList(SListHeader* header, SListEntry* entry);
SListEntry* PopEntrySList(SListHeader* header);