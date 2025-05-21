#include "pch.h"
#include "DBModel.h"
#include <algorithm>
#include <regex>

using namespace DBModel;

String Column::CreateText()
{
    return Helper::Format(
        L"[%s] %s %s %s",
        name.c_str(),
        typeText.c_str(),
        nullable ? L"NULL" : L"NOT NULL",
        identity ? Helper::Format(L"IDENTITY(%d, %d)", seedValue, incrementValue).c_str() : L"");
}



String Index::GetUniqueName()
{
    String result;
    result += primaryKey ? L"PK " : L" ";
    result += uniqueConstraint ? L"UK " : L" ";
    result += (type == IndexType::Clustered ? L"C " : L"NC ");

    for (const ColumnRef& column : columns) {
        result += L"*";
        result += column->name;
        result += L" ";
    }
    return result;
}

String Index::CreateName(const String& tableName)
{
    String result = L"IX_" + tableName;
    for (const ColumnRef& column : columns) {
        result += L"_";
        result += column->name;
    }
    return result;
}

String Index::GetTypeText()
{
    return type == IndexType::Clustered ? L"CLUSTERED" : L"NONCLUSTERED";
}

String Index::GetKeyText()
{
    if (primaryKey) {
        return L"PRIMARY KEY";
    }

    if (uniqueConstraint) {
        return L"UNIQUE";
    }
    return L"";
}

String Index::CreateColumnsText()
{
    String result;

    const int32 size = static_cast<int32>(columns.size());
    for (int32 i = 0; i < size; i++) {
        if (i > 0) {
            result += L", ";
        }
        result += DBModel::Helper::Format(L"[%s]", columns[i]->name.c_str());
    }
    return result;
}

bool Index::DependsOn(const String& columnName)
{
   auto findIt = std::find_if(columns.begin(), columns.end(), [&](const ColumnRef& column) {
        return column->name == columnName;
        });

    return findIt != columns.end();
}

ColumnRef Table::FindColumn(const String& columnName)
{
    
    auto findIt = std::find_if(columns.begin(), columns.end(), [&](const ColumnRef& column) {
        return column->name == columnName;
        });

    if (findIt == columns.end()) {
        return nullptr;
    }


    return *findIt;
}

String Procedure::GenerateCreateQuery()
{
    const WCHAR* query = L"CREATE PROCEDURE [dbo].[%s] %s \nAS BEGIN \n%s \nEND";
    String paramString = GenerateParamString();

    return DBModel::Helper::Format(query, name.c_str(), paramString.c_str(), body.c_str());
}

String Procedure::GenerateAlterQuery()
{
    const WCHAR* query = L"ALTER PROCEDURE [dbo].[%s] %s AS BEGIN %s END";
    String paramString = GenerateParamString();

    return DBModel::Helper::Format(query, name.c_str(), paramString.c_str(), body.c_str());
}

String Procedure::GenerateParamString()
{
    String result;
    const int32 size = parameters.size();
    if (size > 0) {
        result += L"\n";
    }
    for (int32 i = 0; i < size; i++) {
        if (size - 1 > i) {
            result += DBModel::Helper::Format(L"\t%s %s,\n", parameters[i].name.c_str(), parameters[i].type.c_str());
        }
        else {
            result += DBModel::Helper::Format(L"\t%s %s", parameters[i].name.c_str(), parameters[i].type.c_str());
        }
    }

    return result;
}

String Helper::Format(const WCHAR* format, ...)
{
    WCHAR buf[4096];

    va_list ap;
    va_start(ap, format);
    ::vswprintf_s(buf, 4096, format, ap);
    va_end(ap);

    return String(buf);
}

String Helper::DataType2String(DataType type)
{
    switch (type) {
    case DataType::TinyInt:		return L"TinyInt";
    case DataType::SmallInt:	return L"SmallInt";
    case DataType::Int:			return L"Int";
    case DataType::Real:		return L"Real";
    case DataType::DateTime:	return L"DateTime";
    case DataType::Float:		return L"Float";
    case DataType::Bit:			return L"Bit";
    case DataType::Numeric:		return L"Numeric";
    case DataType::BigInt:		return L"BigInt";
    case DataType::VarBinary:	return L"VarBinary";
    case DataType::VarChar:		return L"Varchar";
    case DataType::Binary:		return L"Binary";
    case DataType::NVarChar:	return L"NVarChar";
    default:					return L"None";
    }
}

String Helper::RemoveWhiteSpace(const String& str)
{
    String result = str;
    result.erase(
        std::remove_if(result.begin(), result.end(), [=](WCHAR ch) { return ::isspace(ch);}),
        result.end());
    
    return result;
}

DataType Helper::String2DataType(const WCHAR* str, OUT int32& maxLen)
{
    std::wregex reg(L"([a-z]+)(\\((max|\\d+)\\))?");
    std::wcmatch ret;

    if (std::regex_match(str, OUT ret, reg) == false)
        return DataType::None;

    if (ret[3].matched)
        maxLen = ::_wcsicmp(ret[3].str().c_str(), L"max") == 0 ? -1 : _wtoi(ret[3].str().c_str());
    else
        maxLen = 0;

    if (::_wcsicmp(ret[1].str().c_str(), L"TinyInt") == 0) return DataType::TinyInt;
    if (::_wcsicmp(ret[1].str().c_str(), L"SmallInt") == 0) return DataType::SmallInt;
    if (::_wcsicmp(ret[1].str().c_str(), L"Int") == 0) return DataType::Int;
    if (::_wcsicmp(ret[1].str().c_str(), L"Real") == 0) return DataType::Real;
    if (::_wcsicmp(ret[1].str().c_str(), L"DateTime") == 0) return DataType::DateTime;
    if (::_wcsicmp(ret[1].str().c_str(), L"Float") == 0) return DataType::Float;
    if (::_wcsicmp(ret[1].str().c_str(), L"Bit") == 0) return DataType::Bit;
    if (::_wcsicmp(ret[1].str().c_str(), L"Numeric") == 0) return DataType::Numeric;
    if (::_wcsicmp(ret[1].str().c_str(), L"BigInt") == 0) return DataType::BigInt;
    if (::_wcsicmp(ret[1].str().c_str(), L"VarBinary") == 0) return DataType::VarBinary;
    if (::_wcsicmp(ret[1].str().c_str(), L"Varchar") == 0) return DataType::VarChar;
    if (::_wcsicmp(ret[1].str().c_str(), L"Binary") == 0) return DataType::Binary;
    if (::_wcsicmp(ret[1].str().c_str(), L"NVarChar") == 0) return DataType::NVarChar;
}
