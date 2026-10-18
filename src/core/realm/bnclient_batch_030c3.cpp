// src/core/realm/bnclient_batch_030c3.cpp -- twelve UNMAPPED 1.13c BNClient.dll rows reimplemented from the
// decompiles in build-wasm/ghidra-bnclient/decomp/0x<RVA>.c (alpha lane). Base 0x6FF20000; every global
// is named at its retail RVA. These are MSVC 2003 (VC7.1) Release CRT leaves: the lock-table initialiser,
// the locale-info free, the unlock thunk, the stack probe, the runtime-message writer and banner, the
// new-handler call, the small-block heap allocator, memset, the ANSI GetStringTypeA pair and the
// codepage-to-codepage conversion. Every kernel32 call (GetLocaleInfoA, GetCPInfo, MultiByteToWideChar,
// WideCharToMultiByte, GetStringTypeA/W, GetLastError, GetStdHandle, WriteFile, GetModuleFileNameA,
// MessageBoxA, LeaveCriticalSection) and every heap primitive is a host seam AFTER the retail arguments,
// which keep their order (src/core/realm/mcp_crt_mem.cpp pattern). The byte-identical D2MCPClient rows
// (src/core/realm/mcp_sbh_alloc.cpp 0x3D23, src/core/fn/d2mcpclient_1503.cpp 0x1503,
// src/core/realm/mcp_tiny_rowex.cpp 0x401F) are the cross-check for the ported bodies.
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <emscripten.h>

// ---------------------------------------------------------------------------------------------------
// 1.13c BNClient+0x030C3 (151 B): __mtinitlocknum(locknum). The lock table is the 8-byte-per-lock array
// at +0x1D090; a non-null entry returns 1 untouched. Otherwise malloc(0x18); a null result sets errno =
// 0xC (ENOMEM) and returns 0; else __lock(10), a re-check of the entry, and
// ___crtInitCritSecAndSpinCount(pcs, 0xFA0) -- success stores pcs in the entry, failure frees pcs, sets
// errno = 0xC and runs the finally (the 0x315A _unlock(10) wrapper), returning 0. The re-check after the
// lock frees pcs and returns 1. Same body as the D2MCPClient 0x1503 and D2Gfx 0x19D1 objects.
struct BnMtInitLockEnv {
    uint32_t* lock_table;                        // +0x1D090  DAT_6ff3d090
    void*    (*malloc)(uint32_t size);           // 0x2544 _malloc
    void     (*free)(void* p);                   // 0x3CF4 _free
    void     (*lock)(int32_t num);               // 0x6265 __lock
    void     (*unlock10)(void);                  // 0x315A the _unlock(10) wrapper
    int32_t  (*init_crit_sec)(void* pcs, uint32_t spin);  // ___crtInitCritSecAndSpinCount
};
// 1.13c BNClient+0x030C3 (151 B, callers 0x3163): the lock-table initialiser described above.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t bn_mtinitlocknum(int32_t lock_num, const BnMtInitLockEnv* env) {
    uint32_t* entry = env->lock_table + lock_num * 2;
    if (*entry != 0) return 1;
    void* pcs = env->malloc(0x18);
    if (pcs == nullptr) return 0;                 // errno = ENOMEM
    env->lock(10);
    if (*entry != 0) {
        env->free(pcs);                           // lost the race: drop the fresh section
    } else {
        if (env->init_crit_sec(pcs, 0xFA0) == 0) {
            env->free(pcs);                       // errno = ENOMEM, then the finally unlocks
            env->unlock10();
            return 0;
        }
        *entry = (uint32_t)(uintptr_t)pcs;
    }
    env->unlock10();
    return 1;
}

// ---------------------------------------------------------------------------------------------------
// 1.13c BNClient+0x03601 (208 B): ___freetlocinfo(plocinfo). Free the locale-info block: the collate
// string (+0x2C) and its mon/number tables (+0x34, +0x30) with their helpers, the ctype table set at
// +0x3C (unless it is the default +0x1C668), the time-format string (+0x40) with the block at +0x44
// minus 0xFE, and the lc_time record (+0x50, unless default +0x1C664 or already referenced). Every
// default-global comparison is against the argument values (retail .data), and every callee is a seam.
struct BnFreeTlocinfoEnv {
    uint32_t  default_locinfo;                   // +0x1C668  DAT_6ff3e668
    uint32_t  default_lconv_mon;                 // +0x1F950  DAT_6ff3f950
    uint32_t  default_lconv_num;                 // +0x1F954  DAT_6ff3f954
    uint32_t  default_lc_time;                   // +0x1F94C  DAT_6ff3f94c
    uint32_t  lc_time_global;                    // +0x1C664  DAT_6ff3e664
    void     (*free)(void* p);                   // 0x3CF4 _free
    void     (*free_lconv_mon)(uint32_t locinfo);  // 0x6975 ___free_lconv_mon
    void     (*free_lconv_num)(uint32_t locinfo);  // 0x67E5 ___free_lconv_num
    void     (*free_lc_time)(void* lc_time);       // 0x69D4 ___free_lc_time
};
// 1.13c BNClient+0x03601 (208 B, callers 0x32C6/0x36D1): the locale-info destructor described above.
extern "C"
EMSCRIPTEN_KEEPALIVE void bn_freetlocinfo(uint32_t plocinfo, const BnFreeTlocinfoEnv* env) {
    const uint32_t lconv = *(uint32_t*)(uintptr_t)(plocinfo + 0x3C);
    if (lconv != env->default_locinfo && lconv != 0 &&
        *(uint32_t*)(uintptr_t)(*(uint32_t*)(uintptr_t)(plocinfo + 0x2C)) == 0) {
        uint32_t p = *(uint32_t*)(uintptr_t)(plocinfo + 0x34);
        if (p != 0 && *(uint32_t*)(uintptr_t)p == 0 && p != env->default_lconv_mon) {
            env->free((void*)(uintptr_t)p);
            env->free_lconv_mon(lconv);
        }
        p = *(uint32_t*)(uintptr_t)(plocinfo + 0x30);
        if (p != 0 && *(uint32_t*)(uintptr_t)p == 0 && p != env->default_lconv_num) {
            env->free((void*)(uintptr_t)p);
            env->free_lconv_num(lconv);
        }
        env->free((void*)(uintptr_t)*(uint32_t*)(uintptr_t)(plocinfo + 0x2C));
        env->free((void*)(uintptr_t)lconv);
    }
    uint32_t p = *(uint32_t*)(uintptr_t)(plocinfo + 0x40);
    if (p != env->default_lc_time && p != 0 && *(uint32_t*)(uintptr_t)p == 0) {
        env->free((void*)(uintptr_t)p);
        env->free((void*)(uintptr_t)(*(uint32_t*)(uintptr_t)(plocinfo + 0x44) - 0xFE));
    }
    uint32_t lc_time = *(uint32_t*)(uintptr_t)(plocinfo + 0x50);
    if (lc_time != env->lc_time_global && lc_time != 0 &&
        *(uint32_t*)(uintptr_t)(lc_time + 0xB4) == 0) {          // [0x2D] dwords
        env->free_lc_time((void*)(uintptr_t)lc_time);
        env->free((void*)(uintptr_t)lc_time);
    }
    env->free((void*)(uintptr_t)plocinfo);
}

// ---------------------------------------------------------------------------------------------------
// 1.13c BNClient+0x0407C (9 B): the __unlock(4) thunk -- tail-call the lock release at 0x30AE with
// lock number 4.
extern "C"
EMSCRIPTEN_KEEPALIVE void bn_unlock4(void (*unlock)(int32_t num)) { unlock(4); }

// ---------------------------------------------------------------------------------------------------
// 1.13c BNClient+0x047C0 (61 B): __chkstk. `size` arrives in EAX, `esp` is the entry stack pointer (the
// address the return address was pushed to) and `retaddr` the word the probe stores. Every 0x1000-byte
// page downward is touched so the guard page faults before the frame is entered. Under wasm there is no
// guard page, so the observable is the sequence of stored addresses (D2Client 0x7E60 is the same object).
extern "C"
EMSCRIPTEN_KEEPALIVE void bn_chkstk(uint32_t size, uint8_t* esp, uint32_t retaddr) {
    if (size < 0x1000) {
        *(uint32_t*)(esp - size) = retaddr;
        return;
    }
    uint8_t* p = esp + 4;
    do {
        p -= 0x1000;
        size -= 0x1000;
    } while (size > 0xFFF);
    *(uint32_t*)(p - 4 - size) = retaddr;
}

// ---------------------------------------------------------------------------------------------------
// 1.13c BNClient+0x047FD (376 B): __NMSG_WRITE(code). The message table is the 0x13 eight-byte entries at
// +0x1D590 (code, text); a code that is not found writes nothing. In banner state (__error_mode == 1, or
// 0 with __app_type == 1) the message text goes to stderr (STD_ERROR_HANDLE = -12) through WriteFile.
// Otherwise, unless the code is 0xFC (_RT_CRT_NOTINIT), the program name (GetModuleFileNameA, or
// "<program name unknown>"; truncated to its last three characters replaced by "..." when it would not
// fit in 60 bytes) is concatenated with the fixed "Runtime Error!\n\nProgram: " and "\n\n" text around the
// message and shown via ___crtMessageBoxA (base from 0x6AB0/0x6AC0 are strcpy/strcat).
struct BnNmsgWriteEnv {
    const char* (*message)(int32_t code);            // table lookup at +0x1D590
    uint32_t (*get_std_handle)(int32_t id);          // GetStdHandle
    int32_t  (*write_file)(uint32_t handle, const void* buf, uint32_t count,
                           uint32_t* written);       // WriteFile
    uint32_t (*get_module_file_name)(uint32_t module, char* name, uint32_t size);  // GetModuleFileNameA
    int32_t  (*message_box_a)(const char* text, const char* caption, uint32_t type);  // ___crtMessageBoxA
};
static uint32_t g_bn_exit_done;      // +0x1E374  DAT_6ff3e374 _C_Exit_Done
static uint32_t g_bn_term_done;      // +0x1E378  DAT_6ff3e378 _C_Termination_Done
// 1.13c BNClient+0x047FD (376 B, callers 0x183BA/0x4975/0x2242): the diagnostic writer described above.
extern "C"
EMSCRIPTEN_KEEPALIVE void bn_nmsg_write(int32_t code, const BnNmsgWriteEnv* env) {
    const char* msg = env->message(code);
    if (msg == nullptr) return;
    if (g_bn_exit_done == 1 || (g_bn_exit_done == 0 && g_bn_term_done == 1)) {
        uint32_t written = 0;
        env->write_file(env->get_std_handle(-12), msg, (uint32_t)std::strlen(msg), &written);
    } else if (code != 0xFC) {
        char module[0x104];
        if (env->get_module_file_name(0, module, 0x104) == 0)
            std::strcpy(module, "<program name unknown>");
        const char* name = module;
        const size_t name_len = std::strlen(name);
        if (name_len + 1 > 0x3C) {                // keep room: the tail is replaced by "..."
            name += name_len - 3;
            std::strncpy(const_cast<char*>(name), "...", 3);
        }
        const size_t msg_len = std::strlen(msg);
        char* text = (char*)__builtin_alloca((std::strlen(name) + msg_len + 0x1F) & ~3u);
        std::strcpy(text, "Runtime Error!\n\nProgram: ");
        std::strcat(text, name);
        std::strcat(text, "\n\n");
        std::strcat(text, msg);
        env->message_box_a(text, "Microsoft Visual C++ Runtime Library", 0x12010);
    }
}

// ---------------------------------------------------------------------------------------------------
// 1.13c BNClient+0x04975 (57 B): __FF_MSGBANNER. In banner state (the same __error_mode/__app_type test)
// write the 0xFC and 0xFF runtime messages around the optional host hook _pfnmsg (+0x1E5FC) when set.
struct BnFfMsgBannerEnv {
    void (*nmsg_write)(int32_t code);        // 0x47FD __NMSG_WRITE
    void (*pfnmsg)(void);                    // +0x1E5FC _pfnmsg, may be null
};
// 1.13c BNClient+0x04975 (57 B, caller 0x2242): the banner described above.
extern "C"
EMSCRIPTEN_KEEPALIVE void bn_ff_msgbanner(const BnFfMsgBannerEnv* env) {
    if (!(g_bn_exit_done == 1 || (g_bn_exit_done == 0 && g_bn_term_done == 1))) return;
    env->nmsg_write(0xFC);
    if (env->pfnmsg != nullptr) env->pfnmsg();
    env->nmsg_write(0xFF);
}

// ---------------------------------------------------------------------------------------------------
// 1.13c BNClient+0x049AE (27 B): __callnewh(size). Load the CRT's installed new-handler (+0x1E600); when
// it is non-null call it with the size and answer 1 only if it returns non-zero, else 0. Same body as
// D2MCPClient 0x401F.
// 1.13c BNClient+0x049AE (27 B, callers 0x2275/0x2518/0x3FB6): the new-handler bridge.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t bn_callnewh(uint32_t size, int32_t (*handler)(uint32_t)) {
    if (handler == nullptr) return 0;
    return handler(size) != 0 ? 1 : 0;
}

// ---------------------------------------------------------------------------------------------------
// 1.13c BNClient+0x051F0 (764 B): ___sbh_alloc_block(size). Port of the D2MCPClient 0x3D23 object
// (src/core/realm/mcp_sbh_alloc.cpp): pick a region whose bucket bitmap can serve the request (last-used
// region, then the ring, then any region with a committed group, allocating a region/group when none
// exists), take the head of the first non-empty bucket at or above the request's bucket, split off the
// remainder (bucket clamped to 0x3F), stamp size+1 at the node and the tail word, bump the group's bucket
// count and clear the region cursor when that was its last block. Returns the user pointer (node+4) or 0.
// The two region/group allocators (0x4E0B, 0x4D54) and the six sbh globals are passed in.
struct BnSbhState {
    uint32_t regions;        // +0x20BCC  DAT_6ff40bcc
    uint32_t count;          // +0x20BC8  DAT_6ff40bc8
    uint32_t last_region;    // +0x20BD4  DAT_6ff40bd4
    uint32_t entry;          // +0x20BC4  DAT_6ff40bc4
    uint32_t seq;            // +0x20BDC  DAT_6ff40bdc
};
struct BnSbhAllocBlockEnv {
    BnSbhState* state;
    uint32_t (*new_region)(void);              // 0x4E0B ___sbh_alloc_new_region
    int32_t  (*new_group)(uint32_t region);    // 0x4D54 ___sbh_alloc_new_group
};
static inline uint32_t* bn_p32(uint32_t a) { return (uint32_t*)(uintptr_t)a; }
static inline uint8_t*  bn_p8(uint32_t a)  { return (uint8_t*)(uintptr_t)a; }

// 1.13c BNClient+0x051F0 (764 B, callers 0x249D/0x2275/0x3FB6): the small-block allocator described above.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t bn_sbh_alloc_block(uint32_t size, const BnSbhAllocBlockEnv* env) {
    BnSbhState* s = env->state;
    uint32_t req = (size + 0x17u) & 0xfffffff0u;
    int32_t first_bucket = (int32_t)((size + 0x17u) >> 4) - 1;
    uint32_t end = s->regions + s->count * 0x14u;
    uint8_t b8 = (uint8_t)first_bucket;
    uint32_t region = s->last_region;
    uint32_t lo_mask, hi_mask;
    if (first_bucket < 0x20) {
        lo_mask = 0xffffffffu >> (b8 & 31);
        hi_mask = 0xffffffffu;
    } else {
        lo_mask = 0;
        hi_mask = 0xffffffffu >> ((uint8_t)(b8 - 0x20) & 31);
    }
    while (region < end && ((bn_p32(region)[1] & hi_mask) == 0 && (bn_p32(region)[0] & lo_mask) == 0))
        region += 0x14;
    uint32_t r2 = s->regions;
    if (region == end) {
        while (r2 < s->last_region &&
               ((bn_p32(r2)[1] & hi_mask) == 0 && (bn_p32(r2)[0] & lo_mask) == 0))
            r2 += 0x14;
        region = r2;
        if (r2 == s->last_region) {
            while (r2 < end && bn_p32(r2)[2] == 0) r2 += 0x14;
            uint32_t r3 = s->regions;
            region = r2;
            if (r2 == end) {
                while (r3 < s->last_region && bn_p32(r3)[2] == 0) r3 += 0x14;
                region = r3;
                if (r3 == s->last_region) {
                    region = env->new_region();
                    if (region == 0) return 0;
                }
            }
            int32_t gi = env->new_group(region);
            bn_p32(bn_p32(region + 0x10)[0])[0] = (uint32_t)gi;
            if (bn_p32(bn_p32(region + 0x10)[0])[0] == 0xffffffffu) return 0;
        }
    }
    uint32_t control = bn_p32(region + 0x10)[0];
    int32_t group = (int32_t)bn_p32(control)[0];
    if (group == -1 ||
        ((bn_p32(control + 0xC4 + (uint32_t)group * 4)[0] & hi_mask) == 0 &&
         (bn_p32(control + 0x44 + (uint32_t)group * 4)[0] & lo_mask) == 0)) {
        group = 0;
        uint32_t p = control + 0x44;
        if ((bn_p32(control + 0xC4)[0] & hi_mask) == 0 && (bn_p32(p)[0] & lo_mask) == 0) {
            uint32_t q;
            do {
                q = p + 0x84;
                group++;
                p += 4;
            } while ((bn_p32(q)[0] & hi_mask) == 0 && (bn_p32(p)[0] & lo_mask) == 0);
        }
    }
    uint32_t bucket_base = control + (uint32_t)group * 0x204u + 0x144u;
    int32_t bucket = 0;
    uint32_t bits = bn_p32(control + 0x44 + (uint32_t)group * 4)[0] & lo_mask;
    if (bits == 0) {
        bits = bn_p32(control + 0xC4 + (uint32_t)group * 4)[0] & hi_mask;
        bucket = 0x20;
    }
    while ((int32_t)bits >= 0) { bucket++; bits <<= 1; }
    uint32_t node = bn_p32(bucket_base + (uint32_t)bucket * 8 + 4)[0];
    int32_t rem = (int32_t)bn_p32(node)[0] - (int32_t)req;
    int32_t new_bucket = (rem >> 4) - 1;
    if (new_bucket > 0x3F) new_bucket = 0x3F;
    s->last_region = region;
    if (new_bucket != bucket) {
        if (bn_p32(node)[1] == bn_p32(node)[2]) {
            uint8_t* tally = bn_p8(control + (uint32_t)bucket + 4);
            if (bucket < 0x20) {
                uint32_t mask = ~(0x80000000u >> ((uint8_t)bucket & 31));
                bn_p32(control + 0x44 + (uint32_t)group * 4)[0] &= mask;
                (*tally)--;
                if (*tally == 0) bn_p32(region)[0] &= mask;
            } else {
                uint32_t mask = ~(0x80000000u >> ((uint8_t)(bucket - 0x20) & 31));
                bn_p32(control + 0xC4 + (uint32_t)group * 4)[0] &= mask;
                (*tally)--;
                if (*tally == 0) bn_p32(region)[1] &= mask;
            }
        }
        *(uint32_t*)(uintptr_t)(bn_p32(node)[2] + 4) = bn_p32(node)[1];
        *(uint32_t*)(uintptr_t)(bn_p32(node)[1] + 8) = bn_p32(node)[2];
        if (rem != 0) {
            uint32_t list = bucket_base + (uint32_t)new_bucket * 8;
            bn_p32(node)[2] = list;
            bn_p32(node)[1] = bn_p32(list + 4)[0];
            bn_p32(list + 4)[0] = node;
            *bn_p32(bn_p32(node)[1] + 8) = node;
            if (bn_p32(node)[1] == bn_p32(node)[2]) {
                uint8_t* tally = bn_p8(control + (uint32_t)new_bucket + 4);
                uint8_t was = *tally;
                *tally = (uint8_t)(was + 1);
                uint8_t nb = (uint8_t)new_bucket;
                if (new_bucket < 0x20) {
                    if (was == 0) bn_p32(region)[0] |= 0x80000000u >> (nb & 31);
                    bn_p32(control + 0x44 + (uint32_t)group * 4)[0] |= 0x80000000u >> (nb & 31);
                } else {
                    if (was == 0) bn_p32(region)[1] |= 0x80000000u >> ((uint8_t)(nb - 0x20) & 31);
                    bn_p32(control + 0xC4 + (uint32_t)group * 4)[0] |=
                        0x80000000u >> ((uint8_t)(nb - 0x20) & 31);
                }
            }
        }
    }
    if (rem != 0) {
        bn_p32(node)[0] = (uint32_t)rem;
        *(uint32_t*)(uintptr_t)(node + (uint32_t)rem - 4) = (uint32_t)rem;
    }
    node += (uint32_t)rem;
    bn_p32(node)[0] = req + 1;
    *(uint32_t*)(uintptr_t)(node + req - 4) = req + 1;
    int32_t count = (int32_t)bn_p32(bucket_base)[0];
    bn_p32(bucket_base)[0] = (uint32_t)(count + 1);
    if (count == 0 && region == s->entry && group == (int32_t)s->seq) s->entry = 0;
    bn_p32(control)[0] = (uint32_t)group;
    return node + 4;
}

// ---------------------------------------------------------------------------------------------------
// 1.13c BNClient+0x06C00 (96 B): _memset(dst, val, size). The MSVC 2003 word-wise memset: byte writes to
// reach a 4-byte boundary, replicated-byte dword writes, then the tail bytes. Byte-for-byte the same
// bytes as std::memset; the loop shape is kept because the row is the object.
// 1.13c BNClient+0x06C00 (96 B, callers 0x391E/0x6D18/0x3FB6/0x6FA7): the memset described above.
extern "C"
EMSCRIPTEN_KEEPALIVE void* bn_memset(void* dst, int32_t val, uint32_t size) {
    if (size == 0) return dst;
    uint8_t value = (uint8_t)val;
    uint8_t* p = (uint8_t*)dst;
    if (size > 3) {
        uint32_t head = (0u - (uint32_t)(uintptr_t)dst) & 3;
        uint32_t rest = size;
        if (head != 0) {
            rest = size - head;
            do {
                *p++ = (uint8_t)val;
                head--;
            } while (head != 0);
        }
        uint32_t word = (uint32_t)value * 0x01010101u;
        size = rest & 3;
        uint32_t words = rest >> 2;
        if (words != 0) {
            uint32_t* wp = (uint32_t*)p;
            while (words != 0) {
                *wp++ = word;
                words--;
            }
            p = (uint8_t*)wp;
            if (size == 0) return dst;
        }
    }
    do {
        *p++ = (char)value;
        size--;
    } while (size != 0);
    return dst;
}

// ---------------------------------------------------------------------------------------------------
// 1.13c BNClient+0x06D18 (421 B): ___crtGetStringTypeA(plocinfo, infoType, src, cch, out, code_page,
// bError). On the first call probe GetStringTypeW(1, L"", 1, &w): success marks the wide API available (1),
// ERROR_CALL_NOT_IMPLEMENTED (0x78) marks it absent (2); a failed probe leaves the state 0. State 2 or 0
// takes the ANSI path: the locale is code_page (or the default at +0x1E698), the target is `out` (or the
// default ctype table at +0x1E6A8), the locale's ANSI codepage comes from 0x6F60 -- -1 falls out with 0
// -- and when that codepage differs from code_page the string is first converted through 0x6FA7 (freeing
// the temporary, null conversion returns 0), then GetStringTypeA answers. State 1 takes the wide path:
// MultiByteToWideChar(code_page, 9 or 1, src, cch) sizes a zeroed buffer, the second call fills it, and
// GetStringTypeW answers from it; the buffer is freed. A failed conversion returns 0.
struct BnCrtGetStringTypeAEnv {
    uint32_t default_locale;                           // +0x1E698  DAT_6ff3e698
    uint16_t* pctype_default;                          // +0x1E6A8  DAT_6ff3e6a8 (_pctype)
    uint32_t (*get_last_error)(void);                  // GetLastError
    int32_t  (*get_string_type_w)(uint32_t info_type, const uint16_t* src, int32_t cch,
                                  uint16_t* out);      // GetStringTypeW
    int32_t  (*get_string_type_a)(uint32_t locale, uint32_t info_type, const char* src,
                                  int32_t cch, uint16_t* out);  // GetStringTypeA
    uint32_t (*multi_byte_to_wide_char)(uint32_t cp, uint32_t flags, const char* src,
                                        int32_t cb, uint16_t* out, int32_t cch);  // MultiByteToWideChar
    int32_t  (*locale_ansi_codepage)(uint32_t locale, const void* env);  // 0x6F60
    char*    (*convert)(uint32_t from, uint32_t to, const char* src, uint32_t* pcb,
                        char* outbuf, int32_t outlen, const void* env);  // 0x6FA7
    void*    (*malloc)(uint32_t bytes);
    void     (*free)(void* p);
};
static uint32_t g_bn_gst_wide_state;      // +0x1E6B0  DAT_6ff3e6b0
// 1.13c BNClient+0x06D18 (421 B, callers 0x3852/0x6348): the ANSI/wide GetStringTypeA described above.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t bn_crt_get_string_type_a(uint32_t plocinfo, uint32_t info_type,
        const char* src, int32_t cch, uint16_t* out, uint32_t code_page, int32_t b_error,
        const BnCrtGetStringTypeAEnv* env) {
    if (g_bn_gst_wide_state == 0) {
        uint16_t probe[2] = {0, 0};
        if (env->get_string_type_w(1, (const uint16_t*)u"", 1, probe) != 0)
            g_bn_gst_wide_state = 1;
        else if (env->get_last_error() == 0x78)
            g_bn_gst_wide_state = 2;
    }
    if (g_bn_gst_wide_state == 2 || g_bn_gst_wide_state == 0) {
        uint32_t locale = code_page != 0 ? code_page : env->default_locale;
        uint16_t* dest = out != nullptr ? out : env->pctype_default;
        int32_t ansi_cp = env->locale_ansi_codepage(locale, env);
        if (ansi_cp != -1) {
            char* converted = nullptr;
            if ((uint32_t)ansi_cp != code_page) {
                uint32_t length = (uint32_t)cch;
                converted = env->convert(code_page, (uint32_t)ansi_cp, src, &length, nullptr, 0, env);
                if (converted == nullptr) return 0;
                cch = (int32_t)length;
            }
            int32_t rc = env->get_string_type_a(locale, info_type,
                                                converted != nullptr ? converted : src, cch, dest);
            if (converted != nullptr) env->free(converted);
            return rc;
        }
    } else if (g_bn_gst_wide_state == 1) {
        uint16_t* dest = out != nullptr ? out : env->pctype_default;
        int32_t wide_count = (int32_t)env->multi_byte_to_wide_char(
            code_page, (uint32_t)(b_error != 0) * 8 + 1, src, cch, nullptr, 0);
        if (wide_count != 0) {
            uint16_t* wide = (uint16_t*)env->malloc(((uint32_t)wide_count * 2 + 3) & ~3u);
            if (wide == nullptr) return 0;
            std::memset(wide, 0, (size_t)wide_count * 2);
            int32_t rc = 0;
            if (env->multi_byte_to_wide_char(code_page, 1, src, cch, wide, wide_count) != 0)
                rc = env->get_string_type_w(info_type, wide, wide_count, dest);
            env->free(wide);
            return rc;
        }
    }
    return 0;
}

// ---------------------------------------------------------------------------------------------------
// 1.13c BNClient+0x06F60 (71 B): locale ANSI codepage. GetLocaleInfoA(locale, 0x1004
// (LOCALE_IDEFAULTANSICODEPAGE), buf, 6); a zero return is -1, else atol of the decimal string. This is
// the helper 0x6D18 calls to compare against its code page.
struct BnLocaleAnsiCpEnv {
    int32_t (*get_locale_info_a)(uint32_t locale, uint32_t type, char* buf, int32_t cch);
};
// 1.13c BNClient+0x06F60 (71 B, callers 0x391E/0x6D18): the codepage lookup described above.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t bn_locale_ansi_codepage(uint32_t locale, const BnLocaleAnsiCpEnv* env) {
    char buf[6] = {0, 0, 0, 0, 0, 0};
    if (env->get_locale_info_a(locale, 0x1004, buf, 6) == 0) return -1;
    return (int32_t)std::strtol(buf, nullptr, 10);
}

// ---------------------------------------------------------------------------------------------------
// 1.13c BNClient+0x06FA7 (451 B): multi-byte string conversion between two codepages. Same codepage
// returns null immediately. When GetCPInfo reports MaxCharSize 1 for both codepages the wide length is
// the caller's byte count (a -1 count means strlen+1); otherwise MultiByteToWideChar sizes it, 0 giving
// null. The wide buffer is zeroed (retail _malloca, so a stack buffer here), the string converted into it,
// and then: with no output buffer, the destination size is queried (0 giving null; for the both-single-byte
// case the wide length is reused), a zeroed buffer of that size is taken from the heap, and the bytes are
// written and the caller's length updated (not when it was -1) -- a zero-byte write frees the buffer and
// answers null; with an output buffer, the bytes are written straight into it and that pointer is the
// result, 0 when the write fails. The retail heap-wide branch is dead (`if (stack == 0x64)`), so only the
// caller-owned result buffer can be heap.
struct BnConvertCodepageEnv {
    struct CpInfo { uint32_t max_char_size; uint8_t rest[16]; };
    int32_t  (*get_cp_info)(uint32_t cp, CpInfo* info);              // GetCPInfo
    uint32_t (*multi_byte_to_wide_char)(uint32_t cp, uint32_t flags, const char* src,
                                        int32_t cb, uint16_t* out, int32_t cch);
    uint32_t (*wide_char_to_multi_byte)(uint32_t cp, uint32_t flags, const uint16_t* src,
                                        int32_t cch, char* out, int32_t cb,
                                        const char* default_char, void* used);  // WideCharToMultiByte
    uint32_t (*string_length)(const char* s);                        // 0x60A0 _strlen
    void*    (*calloc)(uint32_t count, uint32_t size);               // 0x3FB6 calloc
    void     (*free)(void* p);                                       // 0x3CF4 _free
};
// 1.13c BNClient+0x06FA7 (451 B, callers 0x391E/0x6D18): the conversion described above.
extern "C"
EMSCRIPTEN_KEEPALIVE char* bn_convert_codepage(uint32_t from, uint32_t to, const char* src,
        uint32_t* pcb, char* outbuf, int32_t outlen, const BnConvertCodepageEnv* env) {
    char* result = nullptr;
    uint32_t cb = *pcb;
    if (from == to) return nullptr;
    BnConvertCodepageEnv::CpInfo info;
    bool both_single = env->get_cp_info(from, &info) != 0 && info.max_char_size == 1 &&
                       env->get_cp_info(to, &info) != 0 && info.max_char_size == 1;
    uint32_t wide_len = cb;
    if (both_single && cb == 0xffffffffu) wide_len = env->string_length(src) + 1;
    if (!both_single) {
        wide_len = env->multi_byte_to_wide_char(from, 1, src, (int32_t)cb, nullptr, 0);
        if (wide_len == 0) return nullptr;
    }
    uint16_t* wide = (uint16_t*)__builtin_alloca(wide_len * 2);
    std::memset(wide, 0, wide_len * 2);
    if (env->multi_byte_to_wide_char(from, 1, src, (int32_t)cb, wide, (int32_t)wide_len) != 0) {
        if (outbuf == nullptr) {
            uint32_t bytes = wide_len;
            if (both_single ||
                (bytes = env->wide_char_to_multi_byte(to, 0, wide, (int32_t)wide_len, nullptr, 0,
                                                      nullptr, nullptr)) != 0) {
                result = (char*)env->calloc(1, bytes);
                if (result != nullptr) {
                    uint32_t written = env->wide_char_to_multi_byte(to, 0, wide, (int32_t)bytes,
                                                                    result, (int32_t)bytes,
                                                                    nullptr, nullptr);
                    if (written == 0) {
                        env->free(result);
                        result = nullptr;
                    } else if (cb != 0xffffffffu) {
                        *pcb = written;
                    }
                }
            }
        } else if (env->wide_char_to_multi_byte(to, 0, wide, (int32_t)wide_len, outbuf, outlen,
                                                nullptr, nullptr) != 0) {
            result = outbuf;
        }
    }
    return result;
}
