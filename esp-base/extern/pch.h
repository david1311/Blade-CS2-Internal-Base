#ifndef PCH_H
#define PCH_H
#define _CRT_SECURE_NO_WARNINGS
#ifndef NOMINMAX
#define NOMINMAX
#endif

#ifndef impl_hpp
#define impl_hpp

#include <Windows.h>
#include <d3d11.h>
#include <dxgi.h>
#include <process.h>
#include <stdlib.h>
#include <time.h>

#ifdef __cplusplus

#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <queue>
#include <stack>
#include <algorithm>
#include <utility>
#include <memory>
#include <functional>
#include <fstream>
#include <sstream>
#include <cmath>
#include <thread>
#include <cstdint>
#endif

#ifdef __cplusplus
#define IMGUI_DEFINE_MATH_OPERATORS
#include <core/misc/logger/logger.h>
#include <core/initialize/initialize.h>
#include <core/overlay/overlay.h>
#include <core/misc/storage/storage.h>
#endif

#endif

#endif
