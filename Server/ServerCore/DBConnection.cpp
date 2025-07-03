#include "pch.h"
#include "DBConnection.h"

bool DBConnection::Connect(SQLHDBC henv, const WCHAR* connectionString)
{
    if (::SQLAllocHandle(SQL_HANDLE_DBC, henv, &_connection) != SQL_SUCCESS) {
        return false;
    }

    WCHAR stringBuffer[MAX_PATH] = { 0 };
    ::wcscpy_s(stringBuffer, connectionString);

    WCHAR resultString[MAX_PATH] = { 0 };
    SQLSMALLINT resultStringLen = 0;

    SQLRETURN result = ::SQLDriverConnectW(
        _connection,
        NULL,
        reinterpret_cast<SQLWCHAR*>(stringBuffer),
        _countof(stringBuffer),
        reinterpret_cast<SQLWCHAR*>(resultString),
        _countof(resultString),
        OUT & resultStringLen,
        SQL_DRIVER_NOPROMPT
    );

    if (::SQLAllocHandle(SQL_HANDLE_STMT, _connection, &_statement) != SQL_SUCCESS) {
        return false;
    }

    return (result == SQL_SUCCESS || result == SQL_SUCCESS_WITH_INFO);
}

void DBConnection::Clear()
{
    if (_connection != SQL_NULL_HANDLE) {
        ::SQLFreeHandle(SQL_HANDLE_DBC, _connection);
        _connection = SQL_NULL_HANDLE;
    }

    if (_statement != SQL_NULL_HANDLE) {
        ::SQLFreeHandle(SQL_HANDLE_STMT, _statement);
        _statement = SQL_NULL_HANDLE;
    }
}

bool DBConnection::Execute(const WCHAR* query)
{
    SQLRETURN result = ::SQLExecDirectW(_statement, (SQLWCHAR*)query, SQL_NTSL);
    if (result == SQL_SUCCESS || result == SQL_SUCCESS_WITH_INFO) {
        return true;
    }

    HandleError(result);
    
    return false;
}

bool DBConnection::Fetch()
{
    SQLRETURN result = ::SQLFetch(_statement);
    switch (result) {
    case SQL_SUCCESS:
    case SQL_SUCCESS_WITH_INFO:
        return true;
    case SQL_NO_DATA:
        return false;
    case SQL_ERROR:
        HandleError(result);
        return false;
    default:
        return true;
    }
}

int32 DBConnection::GetRowCount()
{
    SQLLEN count = 0;
    SQLRETURN result = ::SQLRowCount(_statement, OUT & count);
    if (result == SQL_SUCCESS || SQL_SUCCESS_WITH_INFO) {
        return static_cast<int32>(count);
    }
    return -1;
}

void DBConnection::Unbind()
{
    ::SQLFreeStmt(_statement, SQL_UNBIND);
    ::SQLFreeStmt(_statement, SQL_RESET_PARAMS);
    ::SQLFreeStmt(_statement, SQL_CLOSE);
}

bool DBConnection::BindParam(int32 paramIndex, bool* value, SQLLEN* index)
{
    return BindParam(paramIndex, SQL_C_TINYINT, SQL_TINYINT, size32(bool), value, index);
}

bool DBConnection::BindParam(int32 paramIndex, float* value, SQLLEN* index)
{
    return BindParam(paramIndex, SQL_C_FLOAT, SQL_REAL, 0, value, index);
}

bool DBConnection::BindParam(int32 paramIndex, double* value, SQLLEN* index)
{
    return BindParam(paramIndex, SQL_C_DOUBLE, SQL_DOUBLE, 0, value, index);
}

bool DBConnection::BindParam(int32 paramIndex, int8* value, SQLLEN* index)
{
    return BindParam(paramIndex, SQL_C_TINYINT, SQL_TINYINT, size32(int8), value, index);
}

bool DBConnection::BindParam(int32 paramIndex, int16* value, SQLLEN* index)
{
    return BindParam(paramIndex, SQL_C_SHORT, SQL_SMALLINT, size32(int16), value, index);
}

bool DBConnection::BindParam(int32 paramIndex, int32* value, SQLLEN* index)
{
    return BindParam(paramIndex, SQL_C_LONG, SQL_INTEGER, size32(int32), value, index);
}

bool DBConnection::BindParam(int32 paramIndex, int64* value, SQLLEN* index)
{
    return BindParam(paramIndex, SQL_C_SBIGINT, SQL_BIGINT, size32(int64), value, index);
}

bool DBConnection::BindParam(int32 paramIndex, TIMESTAMP_STRUCT* value, SQLLEN* index)
{
    return BindParam(paramIndex, SQL_C_TYPE_TIMESTAMP, SQL_C_TIMESTAMP, size32(TIMESTAMP_STRUCT), value, index);
}

bool DBConnection::BindParam(int32 paramIndex, const WCHAR* value, SQLLEN* index)
{
    SQLULEN size = static_cast<SQLULEN>((::wcslen(value) + 1) * 2);
    *index = SQL_NTSL;
   
    if (size > WVARCHAR_MAX) {
        return BindParam(paramIndex, SQL_C_WCHAR, SQL_WLONGVARCHAR, size, (SQLPOINTER)value, index);
    }
    else {
        return BindParam(paramIndex, SQL_C_WCHAR, SQL_WVARCHAR, size, (SQLPOINTER)value, index);
    }
}

bool DBConnection::BindParam(int32 paramIndex, const BYTE* value, int32 size, SQLLEN* index)
{
    if (value == nullptr) {
        *index = SQL_NULL_DATA;
        size = 1;
    }
    else {
        *index = size;
    }

    if (size > BINARY_MAX) {
        return BindParam(paramIndex, SQL_C_BINARY, SQL_LONGVARBINARY, size, (BYTE*)value, index);
    }
    else {
        return BindParam(paramIndex, SQL_C_BINARY, SQL_BINARY, size, (BYTE*)value, index);
    }
    
}

bool DBConnection::BindCol(int32 columnIndex, bool* value, SQLLEN* index)
{
    return BindCol(columnIndex, SQL_C_TINYINT, size32(bool), value, index);
}

bool DBConnection::BindCol(int32 columnIndex, float* value, SQLLEN* index)
{
    return BindCol(columnIndex, SQL_C_FLOAT, size32(float), value, index);
}

bool DBConnection::BindCol(int32 columnIndex, double* value, SQLLEN* index)
{
    return BindCol(columnIndex, SQL_C_DOUBLE, size32(double), value, index);
}

bool DBConnection::BindCol(int32 columnIndex, int8* value, SQLLEN* index)
{
    return BindCol(columnIndex, SQL_C_TINYINT, size32(int8), value, index);
}

bool DBConnection::BindCol(int32 columnIndex, int16* value, SQLLEN* index)
{
    return BindCol(columnIndex, SQL_C_SHORT, size32(int16), value, index);
}

bool DBConnection::BindCol(int32 columnIndex, int32* value, SQLLEN* index)
{
    return BindCol(columnIndex, SQL_C_LONG, size32(int32), value, index);
}

bool DBConnection::BindCol(int32 columnIndex, int64* value, SQLLEN* index)
{
    return BindCol(columnIndex, SQL_C_SBIGINT, size32(int64), value, index);
}

bool DBConnection::BindCol(int32 columnIndex, TIMESTAMP_STRUCT* value, SQLLEN* index)
{
    return BindCol(columnIndex, SQL_C_TYPE_TIMESTAMP, size32(TIMESTAMP_STRUCT), value, index);
}

bool DBConnection::BindCol(int32 columnIndex, WCHAR* value, int32 size, SQLLEN* index)
{
    return BindCol(columnIndex, SQL_C_WCHAR, size, value, index);
}

bool DBConnection::BindCol(int32 columnIndex, BYTE* value, int32 size, SQLLEN* index)
{
    return BindCol(columnIndex, SQL_BINARY, size, value, index);
}

bool DBConnection::BindParam(SQLUSMALLINT paramIndex, SQLSMALLINT cType, SQLSMALLINT sqlType, SQLULEN len, SQLPOINTER ptr, SQLLEN* index)
{
    SQLRETURN result = ::SQLBindParameter(_statement, paramIndex, SQL_PARAM_INPUT, cType, sqlType, len, 0, ptr, 0, index);
    if (result != SQL_SUCCESS && result != SQL_SUCCESS_WITH_INFO) {
        HandleError(result);
        return false;
    }

    return true;
}

bool DBConnection::BindCol(SQLUSMALLINT columnIndex, SQLSMALLINT cType, SQLULEN len, SQLPOINTER ptr, SQLLEN* index)
{
    SQLRETURN result = ::SQLBindCol(_statement, columnIndex, cType, ptr, len, index);
    if (result != SQL_SUCCESS && result != SQL_SUCCESS_WITH_INFO) {
        HandleError(result);
        return false;
    }

    return true;
}

void DBConnection::HandleError(SQLRETURN ret)
{
    if (ret == SQL_SUCCESS) {
        return;
    }

    SQLSMALLINT index = 1;
    SQLWCHAR sqlState[MAX_PATH] = { 0 };
    SQLINTEGER nativeErr = 0;
    SQLWCHAR errMsg[MAX_PATH] = { 0 };
    SQLSMALLINT msgLen = 0;
    SQLRETURN errorRet = 0;

    while (true) {
        errorRet= ::SQLGetDiagRecW(
            SQL_HANDLE_STMT, 
            _statement, 
            index, 
            sqlState, 
            OUT & nativeErr, 
            errMsg, 
            _countof(errMsg), 
            OUT & msgLen
        );

        if (errorRet == SQL_NO_DATA) {
            break;
        }

        if (errorRet != SQL_SUCCESS && errorRet != SQL_SUCCESS_WITH_INFO) {
            break;
        }


        wcout.imbue(locale("kor"));
        wcout << errMsg << endl;
        index++;
    }
}
