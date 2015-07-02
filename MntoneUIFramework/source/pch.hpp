#pragma once

#include "targetver.h"

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