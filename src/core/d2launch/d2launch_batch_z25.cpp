// src/core/d2launch/d2launch_batch_z25.cpp -- UNMAPPED 1.13c D2Launch.dll alpha-lane rows (batch z25).
// Retail 32-bit layouts; globals/callees are seams.
#include <cstdint>
#include <cstddef>

struct Z25File { int32_t _ptr, _cnt, _base, _flag, _file; };   // VS2003 FILE
struct Z25Ptd { int32_t _tid, _thandle, _terrno; uint32_t _tdoserrno; };  // _tiddata head

static void* z25_crit[32 * 2];                                  // DAT_6fa60050, stride 2 dwords
static void (*z25_leave_cs)(void*) = nullptr;                   // LeaveCriticalSection
static Z25Ptd* (*z25_getptd)() = nullptr;                       // 0x4714
static int (*z25_flush)(Z25File*) = nullptr;                    // 0x4572
static uint32_t (*z25_commit)(int32_t) = nullptr;               // 0x77C6
static uint32_t (*z25_tls_alloc)() = nullptr;                   // TlsAlloc
static int32_t z25_mbctable_init = 0;                           // DAT_6fa680d8
static void (*z25_initmbctable)() = nullptr;                    // ___initmbctable
static char z25_modname[0x104];                                 // DAT_6fa64fa8
static uint32_t z25_pgmptr, z25_argc, z25_argv;                 // 64e14 / 64df8 / 64dfc
static int32_t z25_wildcard;                                    // DAT_6fa650ac
static uint32_t (*z25_getmodfname)(uint32_t, char*, uint32_t) = nullptr;  // GetModuleFileNameA
static void (*z25_parse_cmdline)(uint32_t* argv, int* argc_bytes) = nullptr;  // 0x59F8/0x4DD2
static void* (*z25_malloc)(size_t) = nullptr;                   // 0x7F25

// 1.13c D2Launch+0x3479
void z25_unlock(int n) { z25_leave_cs(z25_crit[n * 2]); }
// 1.13c D2Launch+0x3525
void z25_unlock10() { z25_unlock(10); }
// 1.13c D2Launch+0x3AFA
int32_t* z25_errno_ptr() { return &z25_getptd()->_terrno; }
// 1.13c D2Launch+0x3B03
uint32_t* z25_doserrno_ptr() { return &z25_getptd()->_tdoserrno; }
// 1.13c D2Launch+0x3C98
void z25_unlock1() { z25_unlock(1); }
// 1.13c D2Launch+0x440B
void z25_unlock4a() { z25_unlock(4); }
// 1.13c D2Launch+0x45CF
int z25_flushall_one(Z25File* f) {
    if (z25_flush(f) != 0) return -1;
    if (f->_flag & 0x4000) return -(z25_commit(f->_file) != 0);
    return 0;
}
// 1.13c D2Launch+0x46DB
void z25_tlsalloc() { z25_tls_alloc(); }
// 1.13c D2Launch+0x48B7
void z25_unlock13() { z25_unlock(0xd); }
// 1.13c D2Launch+0x48C3
void z25_unlock12() { z25_unlock(0xc); }
// 1.13c D2Launch+0x4AB0
void z25_unlock4b() { z25_unlock(4); }
// 1.13c D2Launch+0x4F3E
int z25_setargv() {
    int local8 = 0;  // in_ECX: uninitialised register arg in decompile; GUESSED 0
    if (z25_mbctable_init == 0) z25_initmbctable();
    z25_wildcard = 0;
    z25_getmodfname(0, z25_modname, 0x104);
    z25_pgmptr = (uint32_t)(uintptr_t)z25_modname;
    int in_ecx = local8;
    z25_parse_cmdline(nullptr, &local8);
    uint32_t* p = (uint32_t*)z25_malloc((size_t)(in_ecx + local8 * 4));
    if (!p) return -1;
    z25_parse_cmdline(p, &local8);
    z25_argc = local8 - 1;
    z25_argv = (uint32_t)(uintptr_t)p;
    return 0;
}
