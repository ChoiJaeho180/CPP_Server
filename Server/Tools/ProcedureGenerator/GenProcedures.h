#pragma once
#include "Types.h"
#include <windows.h>
#include "DBBind.h"

namespace SP
{
	
    class InsertGold : public DBBind<3,0>
    {
    public:
    	InsertGold(DBConnection& conn) : DBBind(conn, L"{CALL dbo.spInsertGold(?,?,?)}") { }
    	void In_Gold(int32& v) { BindParam(0, v); };
    	void In_Gold(int32&& v) { _gold = std::move(v); BindParam(0, _gold); };
    	template<int32 N> void In_Name(WCHAR(&v)[N]) { BindParam(1, v); };
    	template<int32 N> void In_Name(const WCHAR(&v)[N]) { BindParam(1, v); };
    	void In_Name(WCHAR* v, int32 count) { BindParam(1, v, count); };
    	void In_Name(const WCHAR* v, int32 count) { BindParam(1, v, count); };
    	void In_CreateDate(TIMESTAMP_STRUCT& v) { BindParam(2, v); };
    	void In_CreateDate(TIMESTAMP_STRUCT&& v) { _createDate = std::move(v); BindParam(2, _createDate); };

    private:
    	int32 _gold = {};
    	TIMESTAMP_STRUCT _createDate = {};
    };

    class GetGold : public DBBind<1,4>
    {
    public:
    	GetGold(DBConnection& conn) : DBBind(conn, L"{CALL dbo.spGetGold(?)}") { }
    	void In_Gold(int32& v) { BindParam(0, v); };
    	void In_Gold(int32&& v) { _gold = std::move(v); BindParam(0, _gold); };
    	void Out_Id(OUT int32& v) { BindCol(0, v); };
    	void Out_Gold(OUT int32& v) { BindCol(1, v); };
    	template<int32 N> void Out_Name(OUT WCHAR(&v)[N]) { BindCol(2, v); };
    	void Out_CreateDate(OUT TIMESTAMP_STRUCT& v) { BindCol(3, v); };

    private:
    	int32 _gold = {};
    };

    class GetOrCreatePlayer : public DBBind<1,8>
    {
    public:
    	GetOrCreatePlayer(DBConnection& conn) : DBBind(conn, L"{CALL dbo.spGetOrCreatePlayer(?)}") { }
    	template<int32 N> void In_Name(WCHAR(&v)[N]) { BindParam(0, v); };
    	template<int32 N> void In_Name(const WCHAR(&v)[N]) { BindParam(0, v); };
    	void In_Name(WCHAR* v, int32 count) { BindParam(0, v, count); };
    	void In_Name(const WCHAR* v, int32 count) { BindParam(0, v, count); };
    	void Out_PlayerId(OUT int64& v) { BindCol(0, v); };
    	template<int32 N> void Out_Name(OUT WCHAR(&v)[N]) { BindCol(1, v); };
    	void Out_MapId(OUT int32& v) { BindCol(2, v); };
    	void Out_X(OUT float& v) { BindCol(3, v); };
    	void Out_Y(OUT float& v) { BindCol(4, v); };
    	void Out_Level(OUT int32& v) { BindCol(5, v); };
    	void Out_Exp(OUT int32& v) { BindCol(6, v); };
    	void Out_Hp(OUT int32& v) { BindCol(7, v); };

    private:
    };


     
};