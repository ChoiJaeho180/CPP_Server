#pragma once

// pre compile header

#ifdef _DEBUG
#pragma comment(lib, "Debug\\ServerCore.lib");
#else
#pragma comment(lib, "Release\\ServerCore.lib");
#endif



#include "CorePch.h"
