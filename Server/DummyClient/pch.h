#pragma once

#ifdef _DEBUG
#pragma comment(lib, "Debug\\ServerCore.lib")
#else
#pragma comment(lib, "Release\\ServerCore.lib")
#endif

#include <winsock2.h>
#include <mswsock.h>
#include <ws2tcpip.h>

#include "CorePch.h"
