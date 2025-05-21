#include "pch.h"
#include "DBSynchronizer.h"
#include "DBBind.h"
#include "CoreGlobal.h"
#include "XmlParser.h"
#include <regex>

namespace SP {
	const WCHAR* QTablesAndColumns =
		L"	SELECT c.object_id, t.name AS tableName, c.name AS columnName, c.column_id, c.user_type_id, c.max_length,"
		"		c.is_nullable, c.is_identity, CAST(ic.seed_value AS BIGINT) AS seedValue, CAST(ic.increment_value AS BIGINT) AS incValue,"
		"		c.default_object_id, dc.definition as defaultDefinition, dc.name as defaultConstraintName"
		"	FROM sys.columns AS c"
		"	JOIN sys.tables AS t"
		"		ON c.object_id = t.object_id"
		"	LEFT JOIN sys.default_constraints AS dc"
		"		ON c.default_object_id = dc.object_id"
		"	LEFT JOIN sys.identity_columns AS ic"
		"		ON c.object_id = ic.object_id AND c.column_id = ic.column_id"
		"	WHERE t.type = 'U'"
		"	ORDER BY object_id ASC, column_id ASC;";

	class GetDBTables : public DBBind<0, 13> 
	{
	public:
		GetDBTables(DBConnection& dbConn): DBBind(dbConn, QTablesAndColumns) {}

		void Out_ObjectId(OUT int32& value) { BindCol(0, value); }
		template<int32 N> void Out_TableName(OUT WCHAR(&value)[N]) { BindCol(1, value); }
		template<int32 N> void Out_ColumnName(OUT WCHAR(&value)[N]) { BindCol(2, value); }
		void Out_ColumnId(OUT int32& value) { BindCol(3, value); }
		void Out_UserType(OUT int32& value) { BindCol(4, value); }
		void Out_MaxLength(OUT int32& value) { BindCol(5, value); }
		void Out_IsNullable(OUT bool& value) { BindCol(6, value); }
		void Out_IsIdentity(OUT bool& value) { BindCol(7, value); }
		void Out_SeedValue(OUT int64& value) { BindCol(8, value); }
		void Out_IncrementValue(OUT int64& value) { BindCol(9, value); }
		void Out_DefaultObjectId(OUT int32& value) { BindCol(10, value); }
		template<int32 N> void Out_DefaultDefinition(OUT WCHAR(&value)[N]) { BindCol(11, value); }
		template<int32 N> void Out_DefaultConstraintName(OUT WCHAR(&value)[N]) { BindCol(12, value); }
	};

	const WCHAR* QIndexes =
		L"	SELECT i.object_id, i.name as indexName, i.index_id, i.type, i.is_primary_key,"
		"		i.is_unique_constraint, ic.column_id, COL_NAME(ic.object_id, ic.column_id) as columnName"
		"	FROM sys.indexes AS i"
		"	JOIN sys.index_columns AS ic"
		"		ON i.object_id = ic.object_id AND i.index_id = ic.index_id"
		"	WHERE i.type > 0 AND i.object_id IN(SELECT object_id FROM sys.tables WHERE type = 'U')"
		"	ORDER BY i.object_id ASC, i.index_id ASC;";
	class GetDBIndexes : public DBBind<0, 8>
	{
	public:
		GetDBIndexes(DBConnection& conn) : DBBind(conn, QIndexes) {}

		void Out_ObjectId(OUT int32& value) { BindCol(0, value); }
		template<int32 N> void Out_IndexName(OUT WCHAR(&value)[N]) { BindCol(1, value); }
		void Out_IndexId(OUT int32& value) { BindCol(2, value); }
		void Out_IndexType(OUT int32& value) { BindCol(3, value); }
		void Out_IsPrimaryKey(OUT bool& value) { BindCol(4, value); }
		void Out_IsUniqueConstraint(OUT bool& value) { BindCol(5, value); }
		void Out_ColumnId(OUT int32& value) { BindCol(6, value); }
		template<int32 N> void Out_ColumnName(OUT WCHAR(&value)[N]) { BindCol(7, value); }
	};

	const WCHAR* QStoredProcedures =
		L"	SELECT name, OBJECT_DEFINITION(object_id) AS body FROM sys.procedures;";
	class GetDBStoredProcedures : public DBBind<0, 2>
	{
	public:
		GetDBStoredProcedures(DBConnection& conn) : DBBind(conn, QStoredProcedures) {}

		template<int32 N> void Out_Name(OUT WCHAR(&value)[N]) { BindCol(0, value); }
		void Out_Body(OUT WCHAR* value, int32 len) { BindCol(1, value, len); }
	};
}



DBSynchronizer::~DBSynchronizer()
{
}


bool DBSynchronizer::Synchronize(const WCHAR* path)
{
	ParseXmlDB(path);

	GatherDBTables();
	GatherDBIndexs();
	GatherDBStoredProcedures();

	CompareDBModel();

	ExecuteUpdateQueries();

	return true;
}

void DBSynchronizer::ParseXmlDB(const WCHAR* path)
{
	XmlNode root;
	XmlParser parser;

	ASSERT_CRASH(parser.ParseFromFile(path, root));

	Vector<XmlNode> tables = root.FindChildren(L"Table");
	for (XmlNode& table : tables) {
		DBModel::TableRef t = MakeShared<DBModel::Table>();
		t->name = table.GetStringAttr(L"name");

		Vector<XmlNode> columns = table.FindChildren(L"Column");
		for (XmlNode& column : columns) {
			DBModel::ColumnRef c = MakeShared<DBModel::Column>();
			c->name = column.GetStringAttr(L"name");
			
			c->typeText = column.GetStringAttr(L"type");
			c->type = DBModel::Helper::String2DataType(c->typeText.c_str(), OUT c->maxLength);
			ASSERT_CRASH(c->type != DBModel::DataType::None);

			c->nullable = !column.GetBoolAttr(L"notnull");
			const WCHAR* identityStr = column.GetStringAttr(L"identity");
			if (::wcslen(identityStr) > 0){
				std::wregex pt(L"(\\d+),(\\d+)");
				std::wcmatch match;
				ASSERT_CRASH(std::regex_match(identityStr, OUT match, pt));
				c->seedValue = _wtoi(match[1].str().c_str());
				c->incrementValue = _wtoi(match[2].str().c_str());
				c->identity = true;
			}
			c->defaultValue = column.GetStringAttr(L"default");
			t->columns.push_back(c);
		}


		Vector<XmlNode> indexs = table.FindChildren(L"Index");
		for (XmlNode& index : indexs) {
			DBModel::IndexRef i = MakeShared<DBModel::Index>();
			const WCHAR* typeStr = index.GetStringAttr(L"type");
			if (::_wcsicmp(typeStr, L"clustered") == 0) {
				i->type = DBModel::IndexType::Clustered;
			}
			else if (::_wcsicmp(typeStr, L"nonclustered") == 0) {
				i->type = DBModel::IndexType::NonClustered;
			}
			else {
				CRASH("Invalid Index Type");
			}
			i->primaryKey = index.FindChild(L"PrimaryKey").IsValid();
			i->uniqueConstraint = index.FindChild(L"UniqueKey").IsValid();
			Vector<XmlNode> columns = index.FindChildren(L"Column");
			for (XmlNode& column : columns) {
				const WCHAR* nameStr = column.GetStringAttr(L"name");
				DBModel::ColumnRef c = t->FindColumn(nameStr);
				ASSERT_CRASH(c != nullptr);
				i->columns.push_back(c);
			}

			t->indexes.push_back(i);
		}
		_xmlTables.push_back(t);
	}

	Vector<XmlNode> procedures = root.FindChildren(L"Procedure");
	for (XmlNode& procedure : procedures) {
		DBModel::ProcedureRef p = MakeShared<DBModel::Procedure>();
		p->name = procedure.GetStringAttr(L"name");
		p->body = procedure.FindChild(L"Body").GetStringValue();

		Vector<XmlNode> params = procedure.FindChildren(L"Param");
		for (XmlNode& paramNode : params)
		{
			DBModel::Param param;
			param.name = paramNode.GetStringAttr(L"name");
			param.type = paramNode.GetStringAttr(L"type");
			p->parameters.push_back(param);
		}

		_xmlProcedures.push_back(p);
	}

	Vector<XmlNode> removedTables = root.FindChildren(L"RemovedTable");
	for (XmlNode& removedTable : removedTables) {
		_xmlRemovedTables.insert(removedTable.GetStringAttr(L"name"));
	}
}

bool DBSynchronizer::GatherDBTables()
{
	int32 objectId;
	WCHAR tableName[101] = { 0 };
	WCHAR columnName[101] = { 0 };
	int32 columnId;
	int32 userTypeId;
	int32 maxLength;
	bool isNullable;
	bool isIdentity;
	int64 seedValue;
	int64 incValue;
	int32 defaultObjectId;
	WCHAR defaultDefinition[101] = { 0 };
	WCHAR defaultConstraintName[101] = { 0 };

	SP::GetDBTables getDBTables(_dbConn);
	getDBTables.Out_ObjectId(OUT objectId);
	getDBTables.Out_TableName(OUT tableName);
	getDBTables.Out_ColumnName(OUT columnName);
	getDBTables.Out_ColumnId(OUT columnId);
	getDBTables.Out_UserType(OUT userTypeId);
	getDBTables.Out_MaxLength(OUT maxLength);
	getDBTables.Out_IsNullable(OUT isNullable);
	getDBTables.Out_IsIdentity(OUT isIdentity);
	getDBTables.Out_SeedValue(OUT seedValue);
	getDBTables.Out_IncrementValue(OUT incValue);
	getDBTables.Out_DefaultObjectId(OUT defaultObjectId);
	getDBTables.Out_DefaultDefinition(OUT defaultDefinition);
	getDBTables.Out_DefaultConstraintName(OUT defaultConstraintName);

	if (getDBTables.Execute() == false) {
		return false;
	}
	
	while (getDBTables.Fetch()) {
		DBModel::TableRef table;
		auto findTable = std::find_if(_dbTables.begin(), _dbTables.end(), [=](const DBModel::TableRef& table) {
			return objectId == table->objectId;
			});

		if (findTable == _dbTables.end()) {
			table = MakeShared<DBModel::Table>();
			table->objectId = objectId;
			table->name = tableName;
			_dbTables.push_back(table);
		}
		else {
			table = *findTable;
		}

		DBModel::ColumnRef column = MakeShared<DBModel::Column>();
		{
			column->name = columnName;
			column->columnId = columnId;
			column->type = static_cast<DBModel::DataType>(userTypeId);
			column->typeText = DBModel::Helper::DataType2String(column->type);
			column->maxLength = (column->type == DBModel::DataType::NVarChar ? maxLength / 2 : maxLength);
			column->nullable = isNullable;
			column->identity = isIdentity;
			column->seedValue = (isIdentity ? seedValue : 0);
			column->incrementValue = (isIdentity ? incValue : 0);

			if (defaultObjectId > 0)
			{
				column->defaultValue = defaultDefinition;
				uint64 p = column->defaultValue.find_first_not_of('(');
				column->defaultValue = column->defaultValue.substr(p, column->defaultValue.size() - p * 2);
				column->defaultConstraintName = defaultConstraintName;
			}
		}

		table->columns.push_back(column);
	}
	return true;
}

bool DBSynchronizer::GatherDBIndexs()
{

	int32 objectId;
	WCHAR indexName[101] = { 0 };
	int32 indexId;
	int32 indexType;
	bool isPrimaryKey;
	bool isUniqueConstraint;
	int32 columnId;
	WCHAR columnName[101] = { 0 };

	SP::GetDBIndexes getDBIndexes(_dbConn);
	getDBIndexes.Out_ObjectId(OUT objectId);
	getDBIndexes.Out_IndexName(OUT indexName);
	getDBIndexes.Out_IndexId(OUT indexId);
	getDBIndexes.Out_IndexType(OUT indexType);
	getDBIndexes.Out_IsPrimaryKey(OUT isPrimaryKey);
	getDBIndexes.Out_IsUniqueConstraint(OUT isUniqueConstraint);
	getDBIndexes.Out_ColumnId(OUT columnId);
	getDBIndexes.Out_ColumnName(OUT columnName);

	if (getDBIndexes.Execute() == false)
		return false;

	while (getDBIndexes.Fetch())
	{
		auto findTable = std::find_if(_dbTables.begin(), _dbTables.end(), [=](const DBModel::TableRef& table) { return table->objectId == objectId; });
		ASSERT_CRASH(findTable != _dbTables.end());
		Vector<DBModel::IndexRef>& indexes = (*findTable)->indexes;
		auto findIndex = std::find_if(indexes.begin(), indexes.end(), [indexId](DBModel::IndexRef& index) { return index->indexId == indexId; });
		if (findIndex == indexes.end())
		{
			DBModel::IndexRef index = MakeShared<DBModel::Index>();
			{
				index->name = indexName;
				index->indexId = indexId;
				index->type = static_cast<DBModel::IndexType>(indexType);
				index->primaryKey = isPrimaryKey;
				index->uniqueConstraint = isUniqueConstraint;
			}
			indexes.push_back(index);
			findIndex = indexes.end() - 1;
		}

		// 인덱스가 걸린 column 찾아서 매핑해준다.
		Vector<DBModel::ColumnRef>& columns = (*findTable)->columns;
		auto findColumn = std::find_if(columns.begin(), columns.end(), [columnId](DBModel::ColumnRef& column) { return column->columnId == columnId; });
		ASSERT_CRASH(findColumn != columns.end());
		(*findIndex)->columns.push_back(*findColumn);
	}

	return true;
}

bool DBSynchronizer::GatherDBStoredProcedures()
{
	WCHAR name[101] = { 0 };
	Vector<WCHAR> body(PROCEDURE_MAX_LEN);

	SP::GetDBStoredProcedures getDBStoredProcedures(_dbConn);
	getDBStoredProcedures.Out_Name(OUT name);
	getDBStoredProcedures.Out_Body(OUT & body[0], PROCEDURE_MAX_LEN);

	if (getDBStoredProcedures.Execute() == false)
		return false;

	while (getDBStoredProcedures.Fetch())
	{
		DBModel::ProcedureRef proc = MakeShared<DBModel::Procedure>();
		{
			proc->name = name;
			proc->fullBody = String(body.begin(), std::find(body.begin(), body.end(), 0));
		}
		_dbProcedures.push_back(proc);
	}

	return true;
}

void DBSynchronizer::CompareDBModel()
{
	// 업데이트 목록 초기화.
	_dependentIndexes.clear();
	for (Vector<String>& queries : _updateQueries) {
		queries.clear();
	}

	// XML에 있는 목록을 우선 갖고 온다.
	Map<String, DBModel::TableRef> xmlTableMap;
	for (DBModel::TableRef& xmlTable : _xmlTables) {
		xmlTableMap[xmlTable->name] = xmlTable;
	}

	// DB 테이블을 순회하면서 XML에 정의된 테이블들과 비교한다.
	for (DBModel::TableRef& dbTable : _dbTables) {
		auto findTable = xmlTableMap.find(dbTable->name);
		if (findTable != xmlTableMap.end()) {
			DBModel::TableRef xmlTable = findTable->second;
			CompareTables(dbTable, xmlTable);
			xmlTableMap.erase(findTable);
		}
		else {
			if (_xmlRemovedTables.find(dbTable->name) != _xmlRemovedTables.end()) {
				GConsoleLogger->WriteStdOut(Color::YELLOW, L"Removing Table : [dbo].[%s]\n", dbTable->name.c_str());
				_updateQueries[UpdateStep::DropTable].push_back(DBModel::Helper::Format(L"DROP TABLE [dbo].[%s]", dbTable->name.c_str()));
			}
		}
	}

	// 맵에서 제거되지 않은 XML 테이블 정의는 새로 추가.
	for (auto& mapIt : xmlTableMap) {
		DBModel::TableRef& xmlTable = mapIt.second;

		String columnsStr;
		const int32 size = static_cast<int32>(xmlTable->columns.size());
		for (int32 i = 0; i < size; i++) {
			if (i != 0)
				columnsStr += L",";
			columnsStr += L"\n\t";
			columnsStr += xmlTable->columns[i]->CreateText();
		}

		GConsoleLogger->WriteStdOut(Color::YELLOW, L"Creating Table : [dbo].[%s]\n", xmlTable->name.c_str());
		_updateQueries[UpdateStep::CreateTable].push_back(DBModel::Helper::Format(L"CREATE TABLE [dbo].[%s] (%s)", xmlTable->name.c_str(), columnsStr.c_str()));

		for (DBModel::ColumnRef& xmlColumn : xmlTable->columns)
		{
			if (xmlColumn->defaultValue.empty())
				continue;

			_updateQueries[UpdateStep::DefaultConstraint].push_back(DBModel::Helper::Format(L"ALTER TABLE [dbo].[%s] ADD CONSTRAINT [%s] DEFAULT (%s) FOR [%s]",
				xmlTable->name.c_str(),
				DBModel::Helper::Format(L"DF_%s_%s", xmlTable->name.c_str(), xmlColumn->name.c_str()).c_str(),
				xmlColumn->defaultValue.c_str(),
				xmlColumn->name.c_str()));
		}

		for (DBModel::IndexRef& xmlIndex : xmlTable->indexes)
		{
			GConsoleLogger->WriteStdOut(Color::YELLOW, L"Creating Index : [%s] %s %s [%s]\n", xmlTable->name.c_str(), xmlIndex->GetKeyText().c_str(), xmlIndex->GetTypeText().c_str(), xmlIndex->GetUniqueName().c_str());
			if (xmlIndex->primaryKey || xmlIndex->uniqueConstraint)
			{
				_updateQueries[UpdateStep::CreateIndex].push_back(DBModel::Helper::Format(
					L"ALTER TABLE [dbo].[%s] ADD CONSTRAINT [%s] %s %s (%s)",
					xmlTable->name.c_str(),
					xmlIndex->CreateName(xmlTable->name).c_str(),
					xmlIndex->GetKeyText().c_str(),
					xmlIndex->GetTypeText().c_str(),
					xmlIndex->CreateColumnsText().c_str()));
			}
			else
			{
				_updateQueries[UpdateStep::CreateIndex].push_back(DBModel::Helper::Format(
					L"CREATE %s INDEX [%s] ON [dbo].[%s] (%s)",
					xmlIndex->GetTypeText().c_str(),
					xmlIndex->CreateName(xmlTable->name).c_str(),
					xmlTable->name.c_str(),
					xmlIndex->CreateColumnsText().c_str()));
			}
		}
	}

	CompareStoredProcedures();
}

void DBSynchronizer::CompareTables(DBModel::TableRef dbTable, DBModel::TableRef xmlTable)
{
	// XML에 있는 컬럼 목록을 갖고 온다.
	Map<String, DBModel::ColumnRef> xmlColumnMap;
	for (DBModel::ColumnRef& xmlColumn : xmlTable->columns) {
		xmlColumnMap[xmlColumn->name] = xmlColumn;
	}

	// DB 테이블 컬럼들을 순회하면서 XML에 정의된 컬럼들과 비교한다.
	for (DBModel::ColumnRef& dbColumn : dbTable->columns) {
		auto findColumn = xmlColumnMap.find(dbColumn->name);
		if (findColumn != xmlColumnMap.end()) {
			DBModel::ColumnRef& xmlColumn = findColumn->second;
			CompareColumns(dbTable, dbColumn, xmlColumn);
			xmlColumnMap.erase(findColumn);
		}
		else {
			GConsoleLogger->WriteStdOut(Color::YELLOW, L"Dropping Column : [%s].[%s]\n", dbTable->name.c_str(), dbColumn->name.c_str());
			if (dbColumn->defaultConstraintName.empty() == false) {
				_updateQueries[UpdateStep::DropColumn].push_back(DBModel::Helper::Format(L"ALTER TABLE [dbo].[%s] DROP CONSTRAINT [%s]", dbTable->name.c_str(), dbColumn->defaultConstraintName.c_str()));
			}

			_updateQueries[UpdateStep::DropColumn].push_back(DBModel::Helper::Format(L"ALTER TABLE [dbo].[%s] DROP COLUMN [%s]", dbTable->name.c_str(), dbColumn->name.c_str()));
		}
	}

	// 맵에서 제거되지 않은 XML 컬럼 정의는 새로 추가.
	for (auto& mapIt : xmlColumnMap) {
		DBModel::ColumnRef& xmlColumn = mapIt.second;
		DBModel::Column newColumn = *xmlColumn;
		newColumn.nullable = true;

		GConsoleLogger->WriteStdOut(Color::YELLOW, L"Adding Column : [%s].[%s]\n", dbTable->name.c_str(), xmlColumn->name.c_str());

		_updateQueries[UpdateStep::AddColumn].push_back(DBModel::Helper::Format(L"ALTER TABLE [dbo].[%s] ADD %s %s",
			dbTable->name.c_str(), xmlColumn->name.c_str(), xmlColumn->typeText.c_str()));

		if (xmlColumn->nullable == false && xmlColumn->defaultValue.empty() == false) {
			_updateQueries[UpdateStep::AddColumn].push_back(DBModel::Helper::Format(L"SET NOCOUNT ON; UPDATE [dbo].[%s] SET [%s] = %s WHERE [%s] IS NULL",
				dbTable->name.c_str(), xmlColumn->name.c_str(), xmlColumn->defaultValue.c_str(), xmlColumn->name.c_str()));
		}

		if (xmlColumn->nullable == false) {
			_updateQueries[UpdateStep::AddColumn].push_back(DBModel::Helper::Format(L"ALTER TABLE [dbo].[%s] ALTER COLUMN %s",
				dbTable->name.c_str(), xmlColumn->CreateText().c_str()));
		}

		if (xmlColumn->defaultValue.empty() == false) {
			_updateQueries[UpdateStep::AddColumn].push_back(DBModel::Helper::Format(L"ALTER TABLE [dbo].[%s] ADD CONSTRAINT [DF_%s_%s] DEFAULT (%s) FOR [%s]",
				dbTable->name.c_str(), dbTable->name.c_str(), xmlColumn->name.c_str(), xmlColumn->defaultValue.c_str(), xmlColumn->name.c_str()));
		}
	}

	// XML에 있는 인덱스 목록을 갖고 온다.
	Map<String, DBModel::IndexRef> xmlIndexMap;
	for (DBModel::IndexRef& xmlIndex : xmlTable->indexes) {
		xmlIndexMap[xmlIndex->GetUniqueName()] = xmlIndex;
	}

	// DB에 실존하는 테이블 인덱스들을 돌면서 XML에 정의된 인덱스들과 비교한다.
	for (DBModel::IndexRef& dbIndex : dbTable->indexes) {
		auto findIndex = xmlIndexMap.find(dbIndex->GetUniqueName());
		if (findIndex != xmlIndexMap.end() && _dependentIndexes.find(dbIndex->GetUniqueName()) == _dependentIndexes.end()) {
			DBModel::IndexRef xmlIndex = findIndex->second;
			xmlIndexMap.erase(findIndex);
		}
		else {
			GConsoleLogger->WriteStdOut(Color::YELLOW, L"Dropping Index : [%s] [%s] %s %s\n", dbTable->name.c_str(), dbIndex->name.c_str(), dbIndex->GetKeyText().c_str(), dbIndex->GetTypeText().c_str());
			if (dbIndex->primaryKey || dbIndex->uniqueConstraint) {
				_updateQueries[UpdateStep::DropIndex].push_back(DBModel::Helper::Format(L"ALTER TABLE [dbo].[%s] DROP CONSTRAINT [%s]", dbTable->name.c_str(), dbIndex->name.c_str()));
			}
			else {
				_updateQueries[UpdateStep::DropIndex].push_back(DBModel::Helper::Format(L"DROP INDEX [%s] ON [dbo].[%s]", dbIndex->name.c_str(), dbTable->name.c_str()));
			}
		}
	}

	// 맵에서 제거되지 않은 XML 인덱스 정의는 새로 추가.
	for (auto& mapIt : xmlIndexMap) {
		DBModel::IndexRef xmlIndex = mapIt.second;
		GConsoleLogger->WriteStdOut(Color::YELLOW, L"Creating Index : [%s] %s %s [%s]\n", dbTable->name.c_str(), xmlIndex->GetKeyText().c_str(), xmlIndex->GetTypeText().c_str(), xmlIndex->GetUniqueName().c_str());
		if (xmlIndex->primaryKey || xmlIndex->uniqueConstraint) {
			_updateQueries[UpdateStep::CreateIndex].push_back(DBModel::Helper::Format(L"ALTER TABLE [dbo].[%s] ADD CONSTRAINT [%s] %s %s (%s)",
				dbTable->name.c_str(), xmlIndex->CreateName(dbTable->name).c_str(), xmlIndex->GetKeyText().c_str(), xmlIndex->GetTypeText().c_str(), xmlIndex->CreateColumnsText().c_str()));
		}
		else {
			_updateQueries[UpdateStep::CreateIndex].push_back(DBModel::Helper::Format(L"CREATE %s INDEX [%s] ON [dbo].[%s] (%s)",
				xmlIndex->GetTypeText(), xmlIndex->CreateName(dbTable->name).c_str(), dbTable->name.c_str(), xmlIndex->CreateColumnsText().c_str()));
		}
	}

}

void DBSynchronizer::CompareColumns(DBModel::TableRef dbTable, DBModel::ColumnRef dbColumn, DBModel::ColumnRef xmlColumn)
{
	uint8 flag = 0;

	if (dbColumn->type != xmlColumn->type) {
		flag |= ColumnFlag::Type;
	}
	if (dbColumn->maxLength != xmlColumn->maxLength && xmlColumn->maxLength > 0) {
		flag |= ColumnFlag::Length;
	}
	if (dbColumn->nullable != xmlColumn->nullable) {
		flag |= ColumnFlag::Nullable;
	} 
	if (dbColumn->identity != xmlColumn->identity || (dbColumn->identity && dbColumn->incrementValue != xmlColumn->incrementValue)) {
		flag |= ColumnFlag::Identity;
	}
	if (dbColumn->defaultValue != xmlColumn->defaultValue) {
		flag |= ColumnFlag::Default;
	}

	if (flag) {
		GConsoleLogger->WriteStdOut(Color::YELLOW, L"Updating Column [%s] : (%s) -> (%s)\n", dbTable->name.c_str(), dbColumn->CreateText().c_str(), xmlColumn->CreateText().c_str());
	}

	// 연관된 인덱스가 있으면 나중에 삭제하기 위해 기록한다.
	if (flag & (ColumnFlag::Type | ColumnFlag::Length | ColumnFlag::Nullable)) {
		for (DBModel::IndexRef& dbIndex : dbTable->indexes) {
			if (dbIndex->DependsOn(dbColumn->name)) {
				_dependentIndexes.insert(dbIndex->GetUniqueName());
			}
		}

		flag |= ColumnFlag::Default;
	}

	if (flag & ColumnFlag::Default) {
		if (dbColumn->defaultConstraintName.empty() == false) {
			_updateQueries[UpdateStep::AlterColumn].push_back(DBModel::Helper::Format(
				L"ALTER TABLE [dbo].[%s] DROP CONSTRAINT [%s]",
				dbTable->name.c_str(),
				dbColumn->defaultConstraintName.c_str()));
		}
	}

	DBModel::Column newColumn = *dbColumn;
	newColumn.defaultValue = L"";
	newColumn.type = xmlColumn->type;
	newColumn.maxLength = xmlColumn->maxLength;
	newColumn.typeText = xmlColumn->typeText;
	newColumn.seedValue = xmlColumn->seedValue;
	newColumn.incrementValue = xmlColumn->incrementValue;

	if (flag & (ColumnFlag::Type | ColumnFlag::Length | ColumnFlag::Identity)) {
		_updateQueries[UpdateStep::AlterColumn].push_back(DBModel::Helper::Format(
			L"ALTER TABLE [dbo].[%s] ALTER COLUMN %s",
			dbTable->name.c_str(),
			newColumn.CreateText().c_str()));
	}

	newColumn.nullable = xmlColumn->nullable;
	if (flag & ColumnFlag::Nullable)
	{
		if (xmlColumn->defaultValue.empty() == false) {
			_updateQueries[UpdateStep::AlterColumn].push_back(DBModel::Helper::Format(
				L"SET NOCOUNT ON; UPDATE [dbo].[%s] SET [%s] = %s WHERE [%s] IS NULL",
				dbTable->name.c_str(),
				xmlColumn->name.c_str(),
				xmlColumn->name.c_str(),
				xmlColumn->name.c_str()));
		}

		_updateQueries[UpdateStep::AlterColumn].push_back(DBModel::Helper::Format(
			L"ALTER TABLE [dbo].[%s] ALTER COLUMN %s",
			dbTable->name.c_str(),
			newColumn.CreateText().c_str()));
	}

	if (flag & ColumnFlag::Default) {
		if (dbColumn->defaultConstraintName.empty() == false) {
			_updateQueries[UpdateStep::AlterColumn].push_back(DBModel::Helper::Format(
				L"ALTER TABLE [dbo].[%s] ADD CONSTRAINT [%s] DEFAULT (%s) FOR [%s]",
				dbTable->name.c_str(),
				DBModel::Helper::Format(L"DF_%s_%s", dbTable->name.c_str(), dbColumn->name.c_str()).c_str(),
				dbColumn->defaultValue.c_str(), dbColumn->name.c_str()));
		}
	}

}

void DBSynchronizer::CompareStoredProcedures()
{
	// XML에 있는 프로시저 목록을 갖고 온다.
	Map<String, DBModel::ProcedureRef> xmlProceduresMap;
	for (DBModel::ProcedureRef& xmlProcedure : _xmlProcedures) {
		xmlProceduresMap[xmlProcedure->name] = xmlProcedure;
	}

	// DB에 실존하는 테이블 프로시저들을 돌면서 XML에 정의된 프로시저들과 비교한다.
	for (DBModel::ProcedureRef& dbProcedure : _dbProcedures) {
		auto findProcedure = xmlProceduresMap.find(dbProcedure->name);
		if (findProcedure != xmlProceduresMap.end()) {
			DBModel::ProcedureRef xmlProcedure = findProcedure->second;
			String xmlBody = xmlProcedure->GenerateCreateQuery();
			if (DBModel::Helper::RemoveWhiteSpace(dbProcedure->fullBody) != DBModel::Helper::RemoveWhiteSpace(xmlBody)) {
				GConsoleLogger->WriteStdOut(Color::YELLOW, L"Updating Procedure : %s\n", dbProcedure->name.c_str());
				_updateQueries[UpdateStep::StoredProcedure].push_back(xmlProcedure->GenerateAlterQuery());
			}
			xmlProceduresMap.erase(findProcedure);
		}
	}

	// 맵에서 제거되지 않은 XML 프로시저 정의는 새로 추가.
	for (auto& mapIt : xmlProceduresMap) {
		GConsoleLogger->WriteStdOut(Color::YELLOW, L"Updating Procedure : %s\n", mapIt.first.c_str());
		_updateQueries[UpdateStep::StoredProcedure].push_back(mapIt.second->GenerateCreateQuery());
	}
}

void DBSynchronizer::ExecuteUpdateQueries()
{
	for (int32 step = 0; step < UpdateStep::Max; step++)
	{
		for (String& query : _updateQueries[step])
		{
			_dbConn.Unbind();
			bool sucess = _dbConn.Execute(query.c_str());
			std::cout << "Success : " << sucess << endl;
			ASSERT_CRASH(sucess);
		}
	}
}
