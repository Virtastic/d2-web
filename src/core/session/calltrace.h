// Call trace: one record per entry of a ported function, as a line
//     <tick> <Dll>+0x<offset> <args>[ -> <ret>]
// Compiled in only when the engine is built with -DD2_CALLTRACE; otherwise every macro is empty and no code or data
// reaches the player build.
//   D2CT_ENTER(dll, rva, fmt, ...)  appends the entry record; evaluates to a handle for D2CT_RET
//   D2CT_RET(handle, fmt, ...)      appends " -> <ret>" to that record (the return decides behaviour)
// The tick is set by the replay driver (d2_ct_tick); a record is written by the code that runs, never read from a capture.
#pragma once
#include <cstddef>
#include <cstdint>
#ifdef D2_CALLTRACE
size_t d2ct_enter(const char* dll, uint32_t rva, const char* fmt, ...) __attribute__((format(printf, 3, 4)));
void d2ct_ret(size_t handle, const char* fmt, ...) __attribute__((format(printf, 2, 3)));
#define D2CT_ENTER(dll, rva, ...) d2ct_enter(dll, rva, __VA_ARGS__)
#define D2CT_RET(h, ...) d2ct_ret(h, __VA_ARGS__)
#else
#define D2CT_ENTER(dll, rva, ...) ((size_t)0)
#define D2CT_RET(h, ...) ((void)(h))
#endif
