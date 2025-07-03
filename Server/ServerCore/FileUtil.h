#pragma once
class FileUtil
{
public:
	static Vector<BYTE>		ReadFile(const WCHAR* path);
	static String			Convert(string str);
	static std::string		Convert(WCHAR str[]);
};

