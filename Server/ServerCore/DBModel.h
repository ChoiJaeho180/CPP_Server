#pragma once
#include "Types.h"

NAMESPACE_BEGIN(DBModel)

using ColumnRef		= shared_ptr<class Column>;
using IndexRef		= shared_ptr<class Index>;
using TableRef		= shared_ptr<class Table>;
using ProcedureRef	= shared_ptr<class Procedure>;

enum class DataType {
	None = 0,
	TinyInt = 48,
	SmallInt = 52,
	Int = 56,
	Real = 59,
	DateTime = 61,
	Float = 62,
	Bit = 104,
	Numeric = 108,
	BigInt = 127,
	VarBinary = 165,
	VarChar = 167,
	Binary = 173,
	NVarChar = 231,

};

// 컬럼 정보   
class Column {
public:
	// 테이블 정의 또는 수정 시 컬럼을 생성할 때 사용될 text를 만든다.
	String				CreateText();
public:
	String				name;
	int32				columnId = 0; // DB
	DataType			type = DataType::None;
	String				typeText;
	int32				maxLength = 0;
	bool				nullable = false;
	bool				identity = false;
	int64				seedValue = 0;
	int64				incrementValue = 0;
	String				defaultValue;
	String				defaultConstraintName; // DB
};


enum class IndexType {
	Clustered = 1,
	NonClustered = 2
};

class Index {
public:
	String					GetUniqueName();
	String					CreateName(const String& tableName);
	String					GetTypeText();
	String					GetKeyText();
	String					CreateColumnsText();
	bool					DependsOn(const String& columnName);	
public:
	String				name; // name
	int32				indexId = 0;
	IndexType			type = IndexType::NonClustered;
	bool				primaryKey = false;
	bool				uniqueConstraint = false;
	Vector<ColumnRef>	columns;
};

class Table {
public:
	ColumnRef				FindColumn(const String& columnName);
public:
	int32				objectId = 0; // db
	String				name;
	Vector<ColumnRef>	columns;
	Vector<IndexRef>	indexes;
};


struct Param {
	String				name;
	String				type;
};

class Procedure {
public:
	String					GenerateCreateQuery();
	String					GenerateAlterQuery();
	String					GenerateParamString();

public:
	String				name;
	String				fullBody; // db
	String				body; // xml
	Vector<Param>		parameters; //xml
};


class Helper {
public:
	static String			Format(const WCHAR* format, ...);
	static String			DataType2String(DataType type);
	static String			RemoveWhiteSpace(const String& str);
	static DataType 		String2DataType(const WCHAR* str, OUT int32& maxLen);
};

NAMESPACE_END
