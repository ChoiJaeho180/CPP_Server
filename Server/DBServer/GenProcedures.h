#pragma once
#include "Types.h"
#include <windows.h>
#include "DBBind.h"

namespace SP
{
	
    class LoadLobbyPlayersInfo : public DBBind<1,9>
    {
    public:
    	LoadLobbyPlayersInfo(DBConnection& conn) : DBBind(conn, L"{CALL dbo.spLoadLobbyPlayersInfo(?)}") { }
    	template<int32 N> void In_Account(WCHAR(&v)[N]) { BindParam(0, v); };
    	template<int32 N> void In_Account(const WCHAR(&v)[N]) { BindParam(0, v); };
    	void In_Account(WCHAR* v, int32 count) { BindParam(0, v, count); };
    	void In_Account(const WCHAR* v, int32 count) { BindParam(0, v, count); };
    	void Out_PlayerId(OUT int64& v) { BindCol(0, v); };
    	template<int32 N> void Out_NickName(OUT WCHAR(&v)[N]) { BindCol(1, v); };
    	void Out_JobType(OUT int32& v) { BindCol(2, v); };
    	void Out_MapId(OUT int32& v) { BindCol(3, v); };
    	void Out_X(OUT float& v) { BindCol(4, v); };
    	void Out_Y(OUT float& v) { BindCol(5, v); };
    	void Out_Level(OUT int32& v) { BindCol(6, v); };
    	void Out_Exp(OUT int32& v) { BindCol(7, v); };
    	void Out_Hp(OUT int32& v) { BindCol(8, v); };

    private:
    };


     
};