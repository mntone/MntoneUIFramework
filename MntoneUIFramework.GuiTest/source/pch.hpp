#pragma once

#include "targetver.h"

#if _DEBUG
#define _CRTDBG_MAP_ALLOC
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

#include <cstdint>
#include <algorithm>
#include <numeric>
#include <memory>
#include <vector>
#include <unordered_map>
#include <functional>

#include <winapifamily.h>
#define WIN32_LEAN_AND_MEAN
#define NOMETAFILE
#define NOMINMAX
#define NOOPENFILE
#define NOSERVICE
#define NOSOUND
#define NOCOMM
#define NOHELP
#define NOPROFILER
#define NODEFERWINDOWPOS
#define NOMCX
#include <Windows.h>
#include <wrl\client.h>
#include <ShellScalingApi.h>