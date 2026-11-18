#pragma once
// Compat: Storm is StormLib in src/assets; the vendored code only needs the handle types and the
// SStr* string helpers, declared by squall's storm/String.hpp and defined in stubs.cpp.
#include <StormHandles.h>
#include <storm/String.hpp>
#include <stdarg.h>
int SStrVPrintf(char* DstBuf, size_t MaxCount, char* Format, va_list ArgList);
