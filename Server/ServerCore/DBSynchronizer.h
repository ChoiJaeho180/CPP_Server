#pragma once
#include "DBConnection.h"
#include "DBModel.h"

class DBSynchronizer
{
	enum {
		PROCEDURE_MAX_LEN = 10000
	};

	enum UpdateStep : uint8 {
		DropIndex,
		AlterColumn,
		AddColumn,
		CreateTable,
		DefaultConstraint,
		CreateIndex,
		DropColumn,
		DropTable,
		StoredProcedure,

		Max
	};

	enum ColumnFlag : uint8 {
		Type = 1 << 0,
		Nullable = 1 << 1,
		Identity = 1 << 2,
		Default = 1 << 3,
		Length = 1 << 4,
	};

public:
	DBSynchronizer(DBConnection& dbConn) : _dbConn(dbConn) {};
	~DBSynchronizer();

	bool		Synchronize(const WCHAR* path);

public:
	void		ParseXmlDB(const WCHAR* path);
	bool		GatherDBTables();
	bool		GatherDBIndexs();
	bool		GatherDBStoredProcedures();

	void		CompareDBModel();
	void		CompareTables(DBModel::TableRef dbTable, DBModel::TableRef xmlTable);
	void		CompareColumns(DBModel::TableRef dbTable, DBModel::ColumnRef dbColumn, DBModel::ColumnRef xmlColumn);
	void		CompareStoredProcedures();

	void		ExecuteUpdateQueries();
private:
	DBConnection&					_dbConn;

	Vector<DBModel::TableRef>		_xmlTables;
	Vector<DBModel::ProcedureRef>	_xmlProcedures;
	Set<String>						_xmlRemovedTables;

	Vector<DBModel::TableRef>		_dbTables;
	Vector<DBModel::ProcedureRef>	_dbProcedures;

private:
	Set<String>						_dependentIndexes;
	Vector<String>					_updateQueries[UpdateStep::Max];
};

