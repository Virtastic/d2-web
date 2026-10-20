// src/core/realm/mcp_crt_batch_b56.cpp -- twelve UNMAPPED 1.13c D2MCPClient.dll rows reimplemented
// from the decompiles in build-wasm/ghidra-d2mcpclient/decomp/0x<RVA>.c (alpha lane, no spec/vector
// yet). Retail base 0x6FA20000; every module global is a static named at its retail RVA.
//
// All twelve are MSVC 2003 (VC7.1) Release CRT bodies the linker pulled into the DLL: the Fls/TLS
// bootstrap (__freeptd, __mtinit), the stdio table (__ioinit, __ioterm), the command line and
// environment (__setenvp, parse_cmdline, __setargv, ___crtGetEnvironmentStringsA), the heap bootstrap
// (__heap_init, __heap_term), the exception filter (__XcptFilter) and the unwind thunk
// (__global_unwind2). Every kernel32 call (Fls*/Tls*, GetStartupInfoA, GetStdHandle, GetFileType,
// GetModuleFileNameA, GetCommandLineA, the environment blocks, HeapCreate/Free/Destroy, VirtualFree,
// UnhandledExceptionFilter, RtlUnwind) and the per-thread data block are host seams. Prior art: the
// byte-identical VC7.1 bodies in src/core/gfx/d2ddraw_batch_22ed.cpp (parse_cmdline 0x22ED, __setargv
// 0x2459, ___crtGetEnvironmentStringsA 0x24FB, __heap_init 0x2637, __heap_term 0x2688, __XcptFilter
// 0x274D) and src/core/realm/mcp_crt_mem.cpp (the seam-environment shape). The environment struct
// below is the place a test runner installs them.
#include <cstdint>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <emscripten.h>

// --- host seams ----------------------------------------------------------------------------------
// The io table and the startup block it reads.
struct D2McpIoInfo {                 // 0x24 bytes, retail DAT_6fa324c0 block entries
    int32_t osfhnd;                  // +0x00 the OS handle
    uint8_t flags;                   // +0x04 osfile
    uint8_t token;                   // +0x05 0x0A after reset
    uint16_t pad;                    // +0x06
    int32_t refcount;                // +0x08 lockinitflag
    uint8_t cs[0x18];                // +0x0C CRITICAL_SECTION
};
static_assert(sizeof(D2McpIoInfo) == 0x24, "1.13c VC7.1 ioinfo layout");

// STARTUPINFOA field order; wasm64 widens the pointer fields, so only the order is retail's.
struct D2McpStartupInfo {
    uint32_t cb;
    char* lpReserved;
    char* lpDesktop;
    char* lpTitle;
    uint32_t dwX, dwY, dwXSize, dwYSize, dwXCountChars, dwYCountChars, dwFillAttribute, dwFlags;
    uint16_t wShowWindow;
    uint16_t cbReserved2;
    uint8_t* lpReserved2;
    int32_t hStdInput, hStdOutput, hStdError;
};

// __XcptFilter: the per-thread exception-action table.
struct D2McpXcptAction { uint32_t num; uint32_t sig; uint32_t handler; };   // 0xC bytes
struct D2McpXcptPtd {
    D2McpXcptAction* xcptacttab;      // Ghidra's _initaddr
    void*            initarg;         // Ghidra's _initarg: saved exception-pointer scratch
    uint32_t         sig_number;      // Ghidra's _pxcptacttab: the signal passed to a handler
};

typedef uint32_t (*D2McpFlsAllocFn)(void (*)(void*));
typedef int32_t  (*D2McpFlsGetFn)(uint32_t);
typedef int32_t  (*D2McpFlsSetFn)(uint32_t, void*);
typedef uint32_t (*D2McpFlsFreeFn)(uint32_t);

struct D2McpCrtEnv56 {
    // __ioinit / __ioterm
    void*    (*malloc)(uint32_t);                                          // 0x28BC _malloc
    void     (*free)(void*);                                               // 0x1BB4 _free
    void     (*get_startup_info)(D2McpStartupInfo*);                       // GetStartupInfoA
    int32_t  (*get_file_type)(int32_t handle);                             // GetFileType
    int32_t  (*get_std_handle)(int32_t which);                             // GetStdHandle
    int32_t  (*set_handle_count)(uint32_t count);                          // SetHandleCount
    int32_t  (*init_crit_sec)(void* cs, uint32_t spin);                    // 0x27AD
    void     (*delete_crit_sec)(void* cs);                                 // DeleteCriticalSection
    // __setenvp / parse_cmdline / __setargv / ___crtGetEnvironmentStringsA
    int32_t  (*init_mbctable)(void);                                       // 0x34DE ___initmbctable
    uint32_t (*get_module_file_name)(char* buf, uint32_t size);            // GetModuleFileNameA
    const char* (*get_command_line)(void);                                 // GetCommandLineA
    void*       (*get_environment_strings_w)(void);                        // GetEnvironmentStringsW
    const char* (*get_environment_strings_a)(void);                        // GetEnvironmentStrings
    int32_t     (*wide_char_to_multi_byte)(uint32_t cp, const void* wide, int32_t wide_len,
                                           char* out, int32_t out_size);   // WideCharToMultiByte
    uint32_t    (*get_last_error)(void);                                   // GetLastError
    void        (*free_environment_strings_w)(const void* block);          // FreeEnvironmentStringsW
    void        (*free_environment_strings_a)(const char* block);          // FreeEnvironmentStringsA
    // __heap_init / __heap_term
    void*    (*heap_create)(uint32_t options);                             // HeapCreate
    void     (*heap_destroy)(void* heap);                                  // HeapDestroy
    void*    (*heap_alloc)(void* heap, uint32_t flags, uint32_t size);     // HeapAlloc
    void     (*heap_free)(void* heap, uint32_t flags, void* block);        // HeapFree
    void     (*virtual_free)(void* addr, uint32_t size, uint32_t type);    // VirtualFree
    int32_t  (*sbh_heap_init)(uint32_t threshold);                         // 0x34FC ___sbh_heap_init
    // __XcptFilter
    D2McpXcptPtd* (*getptd)(void);                                         // 0x18DE __getptd
    int32_t  (*unhandled_exception_filter)(void* exception_ptrs);          // UnhandledExceptionFilter
    void     (*call_xcpt_handler)(uint32_t handler, uint32_t a, uint32_t b);
    // __global_unwind2
    void     (*rtl_unwind)(void* frame, void* target, void* record, void* value);  // 0x5DCE
};

// Every seam is null here: the caller (or a later vector harness) installs the set it exercises.
static const D2McpCrtEnv56 g_mcp_env56 = {};

// --- __mtinit / __freeptd state -------------------------------------------------------------------
struct D2McpFlsState {
    D2McpFlsAllocFn alloc;   // +0x2AB90 DAT_6fa2ab90
    D2McpFlsGetFn   get;     // +0x2AB94 DAT_6fa2ab94
    D2McpFlsSetFn   set;     // +0x2AB98 DAT_6fa2ab98
    D2McpFlsFreeFn  free;    // +0x2AB9C DAT_6fa2ab9c
    uint32_t        key;     // +0x2158 DAT_6fa2a158
};

struct D2McpMtInitHost {
    int32_t  (*mtinitlocks)(void);                    // 0x1450 __mtinitlocks
    void     (*mtterm)(void);                         // 0x18AE __mtterm
    void*    (*calloc)(uint32_t count, uint32_t size);// 0x1C25 calloc
    int32_t  (*get_module_handle)(const char* name);  // GetModuleHandleA, nonzero on success
    void*    (*get_proc_address)(const char* name);   // GetProcAddress, null when absent
    D2McpFlsGetFn   tls_get;                          // TlsGetValue
    D2McpFlsSetFn   tls_set;                          // TlsSetValue
    D2McpFlsAllocFn tls_alloc;                        // 0x18A5 TlsAlloc thunk
    D2McpFlsFreeFn  tls_free;                         // TlsFree
    void     (*freefls)(void* ptd);                   // 0x194F __freefls@4, the FLS destructor
    uint32_t (*get_current_thread_id)(void);          // GetCurrentThreadId
};

// 1.13c D2MCPClient+0x01A96 (47 B, caller 0x011B8 __CRT_INIT): `__freeptd`. With a live FLS key,
// fetch the calling thread's _tiddata when none was passed, run the freefls destructor on it, and set
// the key's slot to null. The destructor and the resolved FlsGetValue/FlsSetValue are host seams.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_mcp_freeptd(void* ptd, const D2McpFlsState* k,
                                         void (*freefls)(void*)) {
    if (k->key == 0xffffffffu) return;
    if (ptd == nullptr) ptd = (void*)(intptr_t)k->get(k->key);
    freefls(ptd);
    k->set(k->key, nullptr);
}

// 1.13c D2MCPClient+0x01AC5 (239 B, caller 0x011B8 __CRT_INIT): `__mtinit`. Initialise the CRT lock
// table (0x1450); create the per-thread data key from kernel32's Fls* when present, otherwise from the
// Tls* fallbacks (with the 0x18A5 TlsAlloc thunk in the alloc slot); allocate one 0x8C-byte _tiddata
// (0x1C25 calloc), bind it to the key, and seed it with the xcpt-action table pointer 0xA190, the
// rand seed 1, thread handle -1 and the current thread id. Any failure runs __mtterm and returns 0.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_mcp_mtinit(D2McpFlsState* k, const D2McpMtInitHost* host) {
    if (host->mtinitlocks() == 0) {
        host->mtterm();
        return 0;
    }
    if (host->get_module_handle("kernel32.dll") != 0) {
        k->alloc = (D2McpFlsAllocFn)host->get_proc_address("FlsAlloc");
        k->get   = (D2McpFlsGetFn)host->get_proc_address("FlsGetValue");
        k->set   = (D2McpFlsSetFn)host->get_proc_address("FlsSetValue");
        k->free  = (D2McpFlsFreeFn)host->get_proc_address("FlsFree");
        if (k->get == nullptr) {
            k->get   = host->tls_get;
            k->set   = host->tls_set;
            k->alloc = host->tls_alloc;
            k->free  = host->tls_free;
        }
    }
    k->key = k->alloc(host->freefls);
    if (k->key != 0xffffffffu) {
        uint32_t* ptd = (uint32_t*)host->calloc(1, 0x8c);
        if (ptd != nullptr && k->set(k->key, ptd) != 0) {
            ptd[0x15] = 0xA190u;                 // _initaddr, retail 0x6FA2A190
            ptd[5] = 1u;                         // _holdrand
            uint32_t tid = host->get_current_thread_id();
            ptd[1] = 0xffffffffu;                // _thandle
            ptd[0] = tid;                        // _tid
            return 1;
        }
    }
    host->mtterm();
    return 0;
}

// --- __ioinit / __ioterm: the CRT's stdio handle table -------------------------------------------
// +0x324A8 the entry count, +0x324C0 the 0x40-slot block-pointer array (retail's 0x6FA325C0 end).
static uint32_t     g_mcp_io_count;
static D2McpIoInfo* g_mcp_io_blocks[0x40];

static void mcp_reset_io_block(D2McpIoInfo* block) {
    for (uint32_t i = 0; i < 0x20; ++i) {
        block[i].osfhnd = -1;
        block[i].flags = 0;
        block[i].token = 10;
        block[i].refcount = 0;
    }
}

// 1.13c D2MCPClient+0x01CFC (510 B, caller 0x011B8 __CRT_INIT): `__ioinit`. Allocate the first
// 0x480-byte block of 0x20 ioinfo records and reset it, then read STARTUPINFOA: when the parent
// passed a handle block (cbReserved2 bytes of flags followed by handles) grow the block array to the
// requested count and adopt every valid inherited handle, initialising its critical section (spin
// 4000) and bumping its refcount. Records without an inherited handle get the standard input/output/
// error handles (-10/-11/-12) with the same critical-section setup; a closed or unknown-type handle
// is only flagged. Finally publish the entry count with SetHandleCount. Returns 0, or -1 when an
// allocation or a critical-section init fails.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_mcp_ioinit(const D2McpCrtEnv56* env) {
    const D2McpCrtEnv56* e = env ? env : &g_mcp_env56;
    uint8_t* block = (uint8_t*)e->malloc(0x480);
    if (block == nullptr) return -1;
    g_mcp_io_count = 0x20;
    g_mcp_io_blocks[0] = (D2McpIoInfo*)block;
    mcp_reset_io_block((D2McpIoInfo*)block);

    D2McpStartupInfo si{};
    e->get_startup_info(&si);
    if (si.cbReserved2 != 0 && si.lpReserved2 != nullptr) {
        uint32_t reserved = *(uint32_t*)si.lpReserved2;
        const uint8_t* pflags = si.lpReserved2 + 4;
        const uint8_t* phandles = pflags + reserved;    // retail clamps `reserved` only after this
        if (reserved > 0x7ff) reserved = 0x800;
        uint32_t want = reserved;
        if ((int32_t)g_mcp_io_count < (int32_t)want) {
            uint32_t slot = 1;
            do {
                uint8_t* more = (uint8_t*)e->malloc(0x480);
                want = g_mcp_io_count;
                if (more == nullptr) break;
                g_mcp_io_count += 0x20;
                g_mcp_io_blocks[slot] = (D2McpIoInfo*)more;
                mcp_reset_io_block((D2McpIoInfo*)more);
                ++slot;
                want = reserved;
            } while ((int32_t)g_mcp_io_count < (int32_t)reserved);
        }
        uint32_t i = 0;
        if ((int32_t)want > 0) {
            do {
                int32_t handle = *(const int32_t*)phandles;
                uint8_t flags = pflags[i];
                if (handle != -1 && (flags & 1) != 0 &&
                    ((flags & 8) != 0 || e->get_file_type(handle) != 0)) {
                    D2McpIoInfo* rec = g_mcp_io_blocks[i >> 5] + (i & 0x1f);
                    rec->osfhnd = handle;
                    rec->flags = flags;
                    if (e->init_crit_sec(rec->cs, 4000) == 0) return -1;
                    rec->refcount += 1;
                }
                phandles += 4;
                ++i;
            } while ((int32_t)i < (int32_t)want);
        }
    }

    for (uint32_t i = 0; i < 3; ++i) {
        D2McpIoInfo* rec = g_mcp_io_blocks[0] + i;
        if (rec->osfhnd == -1) {
            rec->flags = 0x81;
            int32_t which = i == 0 ? -10 : (i == 1 ? -11 : -12);
            int32_t handle = e->get_std_handle(which);
            int32_t type = handle == -1 ? 0 : e->get_file_type(handle);
            if (handle == -1 || type == 0) {
                rec->flags |= 0x40;
            } else {
                rec->osfhnd = handle;
                if ((type & 0xff) == 2) rec->flags |= 0x40;
                else if ((type & 0xff) == 3) rec->flags |= 8;
                if (e->init_crit_sec(rec->cs, 4000) == 0) return -1;
                rec->refcount += 1;
            }
        } else {
            rec->flags |= 0x80;
        }
    }
    e->set_handle_count(g_mcp_io_count);
    return 0;
}

// 1.13c D2MCPClient+0x01EFA (76 B, caller 0x011B8 __CRT_INIT): `__ioterm`. For every block in the
// pointer array, destroy the critical section of each record whose refcount is nonzero, free the
// block and clear its slot. The entry count is left as it was (retail does the same).
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_mcp_ioterm(const D2McpCrtEnv56* env) {
    const D2McpCrtEnv56* e = env ? env : &g_mcp_env56;
    for (uint32_t slot = 0; slot < 0x40; ++slot) {
        D2McpIoInfo* block = g_mcp_io_blocks[slot];
        if (block == nullptr) continue;
        for (uint32_t i = 0; i < 0x20; ++i)
            if (block[i].refcount != 0) e->delete_crit_sec(block[i].cs);
        e->free(block);
        g_mcp_io_blocks[slot] = nullptr;
    }
}

// --- command line / environment ------------------------------------------------------------------
static uint32_t g_mcp_mbctable_done;        // +0x325D4 DAT_6fa325d4
static char*    g_mcp_environment;          // +0x2AA2C DAT_6fa2aa2c
static char**   g_mcp_envp;                 // +0x2AA04 DAT_6fa2aa04
static int32_t  g_mcp_envp_ready;           // +0x325C8 _DAT_6fa325c8
static uint8_t  g_mcp_mbctable[0x102];      // +0x32280 DAT_6fa32280, lead-byte class +1
static uint32_t g_mcp_env_strings_kind;     // +0x2ACA8 DAT_6fa2aca8: 0 untried, 1 wide, 2 ANSI
static char     g_mcp_module_path[0x104];   // +0x2ABA0 DAT_6fa2aba0
static char*    g_mcp_pgmptr;               // +0x2AA14 _DAT_6fa2aa14 (_pgmptr)
static uint32_t g_mcp_argc;                 // +0x2A9F8 _DAT_6fa2a9f8
static uint32_t* g_mcp_argv;                // +0x2A9FC _DAT_6fa2a9fc
static uint32_t g_mcp_setargv_static;       // +0x2ACA4 DAT_6fa2aca4

static bool mcp_lead_byte(uint8_t b) { return (g_mcp_mbctable[(size_t)b + 1] & 0x04) != 0; }

// 1.13c D2MCPClient+0x01F46 (199 B, caller 0x011B8 __CRT_INIT): `__setenvp`. Ensure the code-page
// table is initialised, then walk the double-NUL-terminated environment block counting the entries
// that are not `=C:=...`-style drive-current-directory markers; allocate count*4+4 bytes for the envp
// vector and copy each kept entry into its own malloc'd string. On reaching the final NUL, release
// the source block, null-terminate envp and set the ready flag (return 0). A missing environment or
// a failed allocation returns -1.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_mcp_setenvp(const D2McpCrtEnv56* env) {
    const D2McpCrtEnv56* e = env ? env : &g_mcp_env56;
    if (g_mcp_mbctable_done == 0) e->init_mbctable();
    if (g_mcp_environment == nullptr) return -1;

    int32_t count = 0;
    for (const char* p = g_mcp_environment; *p != 0; p += std::strlen(p) + 1)
        if (*p != '=') ++count;
    char** out = (char**)e->malloc((uint32_t)count * 4 + 4);
    g_mcp_envp = out;
    if (out == nullptr) return -1;

    for (const char* p = g_mcp_environment;;) {
        if (*p == 0) {
            e->free(g_mcp_environment);
            g_mcp_environment = nullptr;
            *out = nullptr;
            g_mcp_envp_ready = 1;
            return 0;
        }
        size_t len = std::strlen(p);
        if (*p != '=') {
            char* copy = (char*)e->malloc((uint32_t)len + 1);
            *out = copy;
            if (copy == nullptr) {
                e->free(g_mcp_envp);
                g_mcp_envp = nullptr;
                return -1;
            }
            std::memcpy(copy, p, len + 1);
            ++out;
        }
        p += len + 1;
    }
}

// 1.13c D2MCPClient+0x0200D (364 B, caller 0x02179 __setargv): `parse_cmdline`. Two-phase: with argv
// and args null it sizes the argument area and the slot count; with both set it fills them. Retail
// passes cmdstart/args/count in registers; the port takes them as arguments. Returns the character
// count (retail's ESI) and stores the slot count including the final NULL slot in *slots.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_mcp_parse_cmdline(uint32_t* argv, int32_t* slots, const char* cmd,
                                                  char* args) {
    const uint8_t* p = (const uint8_t*)cmd;
    uint8_t* out = (uint8_t*)args;
    int32_t count = 0;
    bool in_quote = false;
    uint8_t c;

    *slots = 1;
    if (argv != nullptr) *argv++ = (uint32_t)(uintptr_t)args;   // 32-bit slots, as retail's array
    do {
        if (*p == '"') {
            in_quote = !in_quote;
            ++p;
            c = '"';
        } else {
            ++count;
            if (out != nullptr) *out++ = *p;
            c = *p;
            ++p;
            if (mcp_lead_byte(c)) {              // a lead byte carries its trail byte along
                ++count;
                if (out != nullptr) *out++ = *p;
                ++p;
            }
            if (c == 0) {                        // the empty command line ends the first item
                --p;
                goto first_item_done;
            }
        }
    } while (in_quote || (c != ' ' && c != '\t'));
    if (out != nullptr) out[-1] = 0;

first_item_done:
    in_quote = false;
    while (*p != 0) {
        while (*p == ' ' || *p == '\t') ++p;
        if (*p == 0) break;
        if (argv != nullptr) *argv++ = (uint32_t)(uintptr_t)out;
        ++*slots;
        for (;;) {
            bool copychar = true;
            uint32_t backslashes = 0;
            while (*p == '\\') { ++backslashes; ++p; }
            if (*p == '"') {
                const uint8_t* next = p;
                if ((backslashes & 1) == 0) {
                    // A doubled quote inside a quoted run is one literal quote.
                    if (!in_quote || (next = p + 1, *next != '"')) { copychar = false; next = p; }
                    in_quote = !in_quote;
                }
                backslashes >>= 1;
                p = next;
            }
            for (; backslashes != 0; --backslashes) {
                if (out != nullptr) *out++ = '\\';
                ++count;
            }
            c = *p;
            if (c == 0 || (!in_quote && (c == ' ' || c == '\t'))) break;
            if (copychar) {
                if (out == nullptr) {
                    if (mcp_lead_byte(c)) { ++p; ++count; }
                } else {
                    if (mcp_lead_byte(c)) { *out++ = c; ++p; ++count; }
                    *out++ = *p;
                }
                ++count;
            }
            ++p;
        }
        if (out != nullptr) *out++ = 0;
        ++count;
    }
    if (argv != nullptr) *argv = 0;
    ++*slots;
    return count;
}

// 1.13c D2MCPClient+0x02179 (162 B, caller 0x011B8 __CRT_INIT): `__setargv`. Ensure the code-page
// table, clear the wildcard word, publish GetModuleFileNameA(0) as _pgmptr, take _acmdln (the module
// path when it is null or empty), size the command line
// with the first parse_cmdline, allocate the argv array and the string area in one block
// (slots*4 + character count), fill both with the second pass, and publish _argc = slots-1 and
// _argv. Returns 0, or -1 when the block cannot be allocated.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_mcp_setargv(const D2McpCrtEnv56* env) {
    const D2McpCrtEnv56* e = env ? env : &g_mcp_env56;
    if (g_mcp_mbctable_done == 0) e->init_mbctable();
    g_mcp_setargv_static = 0;
    e->get_module_file_name(g_mcp_module_path, 0x104);
    g_mcp_pgmptr = g_mcp_module_path;
    const char* cmd = e->get_command_line();     // _acmdln (DAT_6fa325c0)
    if (cmd == nullptr || *cmd == 0) cmd = g_mcp_module_path;   // retail falls back to the module path
    int32_t slots = 0;
    int32_t chars = d2_mcp_parse_cmdline(nullptr, &slots, cmd, nullptr);
    uint32_t* argv = (uint32_t*)e->malloc((uint32_t)chars + (uint32_t)slots * 4);   // ponytail: slots hold 32-bit addresses; fine below 4 GiB
    if (argv == nullptr) return -1;
    d2_mcp_parse_cmdline(argv, &slots, cmd, (char*)(argv + slots));
    g_mcp_argc = (uint32_t)(slots - 1);
    g_mcp_argv = argv;
    return 0;
}

// 1.13c D2MCPClient+0x0221B (290 B, caller 0x011B8 __CRT_INIT): `___crtGetEnvironmentStringsA`.
// Probe GetEnvironmentStringsW once (1 = wide); ERROR_CALL_NOT_IMPLEMENTED (0x78) makes it ANSI (2),
// other errors leave it untried. Walk the chosen block over its double-NUL terminator, convert/copy it
// into one malloc'd block, release the source and return it; 0 on any failure.
extern "C"
EMSCRIPTEN_KEEPALIVE void* d2_mcp_crt_get_environment_strings_a(const D2McpCrtEnv56* env) {
    const D2McpCrtEnv56* e = env ? env : &g_mcp_env56;
    const wchar_t* wide = nullptr;
    if (g_mcp_env_strings_kind == 0) {
        wide = (const wchar_t*)e->get_environment_strings_w();
        if (wide != nullptr) {
            g_mcp_env_strings_kind = 1;
        } else if (e->get_last_error() == 0x78) {
            g_mcp_env_strings_kind = 2;
        }
    }
    if (g_mcp_env_strings_kind != 1) {
        if (g_mcp_env_strings_kind != 2 && g_mcp_env_strings_kind != 0) return nullptr;
        const char* ansi = e->get_environment_strings_a();
        if (ansi == nullptr) return nullptr;
        const char* p = ansi;
        while (*p != 0) { while (*p != 0) ++p; ++p; }
        size_t bytes = (size_t)(p - ansi) + 1;            // plus the final NUL
        void* dst = e->malloc((uint32_t)bytes);
        if (dst != nullptr) std::memcpy(dst, ansi, bytes);
        e->free_environment_strings_a(ansi);
        return dst;
    }
    if (wide == nullptr) {
        wide = (const wchar_t*)e->get_environment_strings_w();
        if (wide == nullptr) return nullptr;
    }
    const wchar_t* p = wide;
    while (*p != 0) { while (*p != 0) ++p; ++p; }
    int32_t chars = (int32_t)(p - wide) + 1;              // includes the final NUL
    int32_t bytes = e->wide_char_to_multi_byte(0, wide, chars, nullptr, 0);
    char* dst = bytes != 0 ? (char*)e->malloc((uint32_t)bytes) : nullptr;
    if (dst != nullptr && e->wide_char_to_multi_byte(0, wide, chars, dst, bytes) == 0) {
        e->free(dst);
        dst = nullptr;
    }
    e->free_environment_strings_w(wide);
    return dst;
}

// --- heap bootstrap ------------------------------------------------------------------------------
static void*    g_mcp_heap;                // +0x324A0 DAT_6fa324a0
static uint32_t g_mcp_heap_type;           // +0x324A4 DAT_6fa324a4 (1 OS heap, 3 small-block heap)
static uint32_t g_mcp_os_platform = 2;     // +0x2A9E4 DAT_6fa2a9e4 (VER_PLATFORM_WIN32_NT)
static uint32_t g_mcp_os_version = 5;      // +0x2A9F0 DAT_6fa2a9f0 (major version)
static void*    g_mcp_sbh_regions;         // +0x32254 DAT_6fa32254: 0x14-byte region records
static uint32_t g_mcp_sbh_count;           // +0x32250 DAT_6fa32250

// 1.13c D2MCPClient+0x02357 (81 B, caller 0x011B8 __CRT_INIT): `__heap_init`. HeapCreate with
// HEAP_NO_SERIALIZE when the mtflag argument is 0; pick the heap kind (___heap_select: OS heap on
// NT5+, else the small-block heap) and on the small-block path also build its region array, 0x3F8
// bytes; a failed region init destroys the heap and returns 0, otherwise 1.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_mcp_heap_init(int32_t mtflag, const D2McpCrtEnv56* env) {
    const D2McpCrtEnv56* e = env ? env : &g_mcp_env56;
    g_mcp_heap = e->heap_create(mtflag == 0 ? 1u : 0u);
    if (g_mcp_heap == nullptr) return 0;
    g_mcp_heap_type = (g_mcp_os_platform == 2 && g_mcp_os_version > 4) ? 1u : 3u;
    if (g_mcp_heap_type == 3 && e->sbh_heap_init(0x3f8) == 0) {
        e->heap_destroy(g_mcp_heap);
        return 0;
    }
    return 1;
}

// 1.13c D2MCPClient+0x023A8 (127 B, caller 0x011B8 __CRT_INIT): `__heap_term`. On the small-block
// path, for every live region release its 0x100000-byte reservation (MEM_DECOMMIT then MEM_RELEASE),
// free the region's block header, then free the region array; finally destroy the process heap.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_mcp_heap_term(const D2McpCrtEnv56* env) {
    const D2McpCrtEnv56* e = env ? env : &g_mcp_env56;
    if (g_mcp_heap_type == 3) {
        uint8_t* region = (uint8_t*)g_mcp_sbh_regions;
        for (uint32_t i = 0; i < g_mcp_sbh_count; ++i, region += 0x14) {
            void* base = *(void**)(region + 0x0c);
            e->virtual_free(base, 0x100000, 0x4000);   // MEM_DECOMMIT
            e->virtual_free(base, 0, 0x8000);          // MEM_RELEASE
            e->heap_free(g_mcp_heap, 0, *(void**)(region + 0x10));
        }
        e->heap_free(g_mcp_heap, 0, g_mcp_sbh_regions);
    }
    e->heap_destroy(g_mcp_heap);
}

// --- exception filter ----------------------------------------------------------------------------
static uint32_t g_mcp_xcpt_first;          // +0x2208 DAT_6fa2a208
static uint32_t g_mcp_xcpt_zero_count;     // +0x220C DAT_6fa2a20c
static uint32_t g_mcp_xcpt_table_len;      // +0x2214 DAT_6fa2a214

// 1.13c D2MCPClient+0x0246D (356 B, caller 0x025D1 the C++ EH filter thunk): `__XcptFilter`. Find
// the current thread's action record for the exception code; no record (or a null handler) goes to
// UnhandledExceptionFilter, handler 5 disarms the record and answers 1, handler 1 answers -1. Any
// other handler is called with the record's signal word -- for signal 8 the zero-count window is
// disarmed first and the exception code is mapped to the CRT signal number stored in the ptd. A null
// handler slot is not re-entered.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_mcp_xcpt_filter(uint32_t exception_num, void* exception_ptrs,
                                                const D2McpCrtEnv56* env) {
    const D2McpCrtEnv56* e = env ? env : &g_mcp_env56;
    D2McpXcptPtd* ptd = e->getptd();
    D2McpXcptAction* tab = ptd->xcptacttab;
    D2McpXcptAction* found = nullptr;
    if (tab != nullptr) {
        D2McpXcptAction* end = tab + g_mcp_xcpt_table_len;
        for (D2McpXcptAction* rec = tab; rec < end; ++rec)
            if (rec->num == exception_num) { found = rec; break; }
    }
    if (found == nullptr || found->handler == 0) return e->unhandled_exception_filter(exception_ptrs);
    if (found->handler == 5) {
        found->handler = 0;
        return 1;
    }
    if (found->handler != 1) {
        void* saved_arg = ptd->initarg;
        ptd->initarg = exception_ptrs;
        if (found->sig == 8) {
            // Retail disarms the handler slots of table entries [first, first+count).
            for (uint32_t i = g_mcp_xcpt_first; i < g_mcp_xcpt_first + g_mcp_xcpt_zero_count; ++i)
                ptd->xcptacttab[i].handler = 0;
            uint32_t code = found->num;
            uint32_t saved_sig = ptd->sig_number;
            if (code == 0xC000008EU)      ptd->sig_number = 0x83;   // SIGFPE invalid
            else if (code == 0xC0000090U) ptd->sig_number = 0x81;   // SIGFPE
            else if (code == 0xC0000091U) ptd->sig_number = 0x84;   // overflow
            else if (code == 0xC0000093U) ptd->sig_number = 0x85;   // underflow
            else if (code == 0xC000008DU) ptd->sig_number = 0x82;   // SIGFPE zero divide
            else if (code == 0xC000008FU) ptd->sig_number = 0x86;   // inexact
            else if (code == 0xC0000092U) ptd->sig_number = 0x8A;   // stack fault
            e->call_xcpt_handler(found->handler, 8, ptd->sig_number);
            ptd->sig_number = saved_sig;
        } else {
            uint32_t handler = found->handler;   // retail loads the code address before disarming
            found->handler = 0;
            e->call_xcpt_handler(handler, found->sig, 0);
        }
        ptd->initarg = saved_arg;
    }
    return -1;
}

// 1.13c D2MCPClient+0x028FC (32 B, no direct caller -- the CRT unwind path reaches it as
// _global_unwind2): RtlUnwind(frame, 0x6FA22914, NULL, NULL). The target is the CRT's own resume
// label, kept as its retail RVA; the call itself is a host seam.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_mcp_global_unwind2(void* frame, const D2McpCrtEnv56* env) {
    const D2McpCrtEnv56* e = env ? env : &g_mcp_env56;
    e->rtl_unwind(frame, (void*)(uintptr_t)0x22914, nullptr, nullptr);
}

// The CRT globals above, for a capture probe (src/core/fn/d2mcpclient_2179.cpp) to pin and read back.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_mcp_crt_state(void** out) {
    out[0] = &g_mcp_mbctable_done; out[1] = g_mcp_mbctable; out[2] = g_mcp_module_path;
    out[3] = &g_mcp_pgmptr; out[4] = &g_mcp_argc; out[5] = &g_mcp_argv; out[6] = &g_mcp_setargv_static;
}

// Probe entry: the four seams __setargv reaches, in one call (the env struct stays private to this file).
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_mcp_setargv_cb(int32_t (*init_mbc)(void),
        uint32_t (*module_file)(char*, uint32_t), const char* (*cmdline)(void), void* (*alloc)(uint32_t)) {
    D2McpCrtEnv56 e = {};
    e.init_mbctable = init_mbc; e.get_module_file_name = module_file;
    e.get_command_line = cmdline; e.malloc = alloc;
    return d2_mcp_setargv(&e);
}
