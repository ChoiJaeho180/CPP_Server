#include "pch.h"
#include "FileUtil.h"
#include <filesystem>
#include <fstream>

/*-----------------
	FileUtils
------------------*/

namespace fs = std::filesystem;

Vector<BYTE> FileUtil::ReadFile(const WCHAR* path)
{
	Vector<BYTE> ret;

	fs::path filePath{ path };

	const uint32 fileSize = static_cast<uint32>(fs::file_size(filePath));
	ret.resize(fileSize);

	basic_ifstream<BYTE> inputStream{ filePath };
	inputStream.read(&ret[0], fileSize);

	return ret;
}

String FileUtil::Convert(string str)
{
	const int32 srcLen = static_cast<int32>(str.size());

	String ret;
	if (srcLen == 0) {
		return ret;
	}

	const int32 retLen = ::MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<char*>(&str[0]), srcLen, NULL, 0);
	ret.resize(retLen);
	::MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<char*>(&str[0]), srcLen, &ret[0], retLen);

	return ret;
}

std::string FileUtil::Convert(WCHAR str[])
{
	char buffer[256] = {};
	::WideCharToMultiByte(CP_UTF8, 0, str, -1, buffer, sizeof(buffer), nullptr, nullptr);
	std::string utf8Name = buffer;
	return utf8Name;
}
