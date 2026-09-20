// Compat shim for the vendored D2MOO DRLG (src/core/d2moo): just enough <Windows.h>
// to compile under emscripten/clang. Calling conventions are no-ops on WebAssembly; the engine
// target is wasm64.
#pragma once
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <stdio.h>
#include <ctype.h>

#define __stdcall
#define __fastcall
#define __cdecl
#define __forceinline inline
#define __int8 char
#define __int16 short
#define __int32 int
#define __int64 long long
#define __declspec(x)
#define _Analysis_assume_(x)
// MSVC SAL lock annotations
#define _Requires_lock_held_(x)
#define _Releases_lock_(x)
#define _Acquires_lock_(x)
#define _Requires_lock_not_held_(x)
#define _Guarded_by_(x)
#define _Post_satisfies_(x)
#define _In_
#define _Out_
#define _Inout_
#define _In_opt_
#define _Out_opt_
#define _Ret_maybenull_
#define _Ret_notnull_
#define _Check_return_
#define __debugbreak() abort()

// D2MOO export decorations: nothing is a DLL here. Defined this early because every vendored
// header reaches Windows.h (via D2BasicTypes.h) and not all of them reach Fog.h.
#define D2COMMON_DLL_DECL
#define FOG_DLL_DECL
#define D2CMP_DLL_DECL
#define D2LANG_DLL_DECL
#define D2WIN_DLL_DECL
#define STORM_DLL_DECL
#define D2GAME_DLL_DECL

typedef uint32_t DWORD;
typedef uint16_t WORD;
typedef uint8_t  BYTE;
typedef int32_t  LONG;
typedef uint32_t UINT;
typedef void*    HANDLE;
typedef void*    HMODULE;
typedef const char* LPCSTR;
typedef char*    LPSTR;
typedef void*    LPVOID;

#define TRUE  1
#define FALSE 0
#define MAX_PATH 260

struct RECT  { LONG left, top, right, bottom; };
struct FILETIME { DWORD dwLowDateTime, dwHighDateTime; };
struct POINT { LONG x, y; };
struct PALETTEENTRY { BYTE peRed, peGreen, peBlue, peFlags; };
struct CRITICAL_SECTION { int32_t unused; };
typedef CRITICAL_SECTION* LPCRITICAL_SECTION;

inline void EnterCriticalSection(LPCRITICAL_SECTION) {}
inline void LeaveCriticalSection(LPCRITICAL_SECTION) {}
typedef void (*FARPROC)();
inline long InterlockedIncrement(long volatile* p) { return *p = *p + 1; }
inline long InterlockedDecrement(long volatile* p) { return *p = *p - 1; }
inline DWORD GetTickCount() { return 0; }
inline void OutputDebugStringA(const char*) {}

#define stricmp  strcasecmp
#define strnicmp strncasecmp
#define _stricmp strcasecmp
#define _strnicmp strncasecmp
// MSVC "secure" CRT, both arities the vendored code uses.
#include <stdarg.h>
#include <limits.h>
template <size_t N> inline void strcpy_s(char (&d)[N], const char* s) { strncpy(d, s, N - 1); d[N - 1] = 0; }
inline void strcpy_s(char* d, size_t n, const char* s) { if (n) { strncpy(d, s, n - 1); d[n - 1] = 0; } }
template <size_t N> inline void strncpy_s(char (&d)[N], const char* s, size_t n) { size_t m = n < N - 1 ? n : N - 1; strncpy(d, s, m); d[m] = 0; }
inline void strncpy_s(char* d, size_t dn, const char* s, size_t n) { if (dn) { size_t m = n < dn - 1 ? n : dn - 1; strncpy(d, s, m); d[m] = 0; } }
template <size_t N> inline int sprintf_s(char (&d)[N], const char* f, ...) { va_list a; va_start(a, f); int r = vsnprintf(d, N, f, a); va_end(a); return r; }
inline int sprintf_s(char* d, size_t n, const char* f, ...) { va_list a; va_start(a, f); int r = vsnprintf(d, n, f, a); va_end(a); return r; }
#define wsprintfA sprintf
#define _strcmpi strcasecmp
inline void InitializeCriticalSection(LPCRITICAL_SECTION) {}
inline void DeleteCriticalSection(LPCRITICAL_SECTION) {}
inline int IsBadCodePtr(FARPROC p) { return p == nullptr; }
inline int IsBadReadPtr(const void* p, size_t) { return p == nullptr; }
