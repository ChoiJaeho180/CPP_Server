#pragma once

enum class Color {
	BLACK,
	WHITE,
	RED,
	GREEN,
	BLUE,
	YELLOW,
};

class ConsoleLog
{
	enum { BUFFER_SIZE = 4096 };

public:
	ConsoleLog();
	~ConsoleLog();

public:
	void				WriteStdOut(Color color, const WCHAR* format, ...);
	void				WriteStdErr(Color color, const WCHAR* format, ...);

protected:
	void				SetColor(bool stdOut, Color color);

private:
	HANDLE			_stdOut;
	HANDLE			_stdErr;
};

