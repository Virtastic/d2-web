// src/core/fog/fog_batch_19040.cpp -- 1.13c Fog.dll alpha-lane rows reimplemented from the
// decompiles in build-wasm/ghidra-fog/decomp/0x<RVA>.c, with the calling conventions and lost
// varargs recovered from the disassembly of retail/D2/D2SE/CORES/1.13c/Fog.dll. Base 0x6FF50000;
// every RVA below is retail's.
//
// These are the exception path, the debug print path and the clock helpers. Their callees are
// module-internal functions that are not reimplemented yet and the Win32 imports the compat shim
// does not carry, so each is a seam: a function pointer on an env struct passed to the function.
// Variadic entries (Fog+0x19040, Fog+0x10D40, Fog+0x10D60, Fog+0x10CD0) must take their env
// first, because the C varargs list has to remain the last parameter.
//
// Globals live at their retail addresses (0x6FF7xxxx / 0x6FFAxxxx) in a zero-filled image; only
// Fog+0x7FFF0 has a non-zero initialiser (-1, checked at .data+0x2FFF0), and that one is what
// makes the InterlockedIncrement guard in Fog+0x11160 run the handler on the first entry.
#include <cstdint>
#include <cstdarg>
#include <cstdio>
#include <cstring>

#include <Windows.h>

typedef void (*fog_log_fn)(void* logfile, const char* fmt, ...);
typedef void (*fog_log_va_fn)(void* logfile, int level, int mode, const char* fmt, va_list ap);
typedef void (*fog_fatal_fn)(int level, const char* file, int line, const char* fmt, ...);
typedef void (*fog_void_fn)(void);
typedef void (*fog_exit_fn)(int code);
typedef void (*fog_thread_dump_fn)(uint32_t thread_handle);

static uint32_t fog_ld32(const void* p) {
    uint32_t v;
    memcpy(&v, p, 4);
    return v;
}
static void fog_st32(void* p, uint32_t v) {
    memcpy(p, &v, 4);
}

// --- Fog+0x19040 ------------------------------------------------------------------------------

typedef uint32_t (*fog_capture_ip_fn)(void);
typedef void (*fog_ordinal116_fn)(void);

struct Fog19040Api {
    fog_capture_ip_fn capture_ip;   // Fog+0xFC20
    fog_ordinal116_fn ordinal116;   // WSOCK32 ordinal 116
    fog_fatal_fn fatal;             // Fog+0x10F30
    fog_void_fn abort;              // Fog+0x184A0
    fog_exit_fn exit_process;       // Fog+0x1BA9, _exit
    fog_log_va_fn log_va;           // Fog+0x18770
    void* default_log;              // &DAT_6ff768ec
};

// 1.13c Fog+0x19040
// Variadic fatal-log entry. A null format is the internal-inconsistency path: capture the
// instruction pointer, call the WSOCK32 ordinal, report level 6 at Fog/Src/Exceptions.cpp:0x14F
// with the captured IP, abort, then _exit(-1). Otherwise pass the whole varargs list to the
// engine logger with level 0 and mode 0, against the default log file.
extern "C"
void fog_sub_19040(const Fog19040Api* api, const char* fmt, ...) {
    if (fmt == nullptr) {
        const uint32_t ip = api->capture_ip();
        api->ordinal116();
        api->fatal(6, "", 0x14f, "Unrecoverable internal error %08x", ip);
        api->abort();
        api->exit_process(-1);
        return;
    }
    va_list ap;
    va_start(ap, fmt);
    api->log_va(api->default_log, 0, 0, fmt, ap);
    va_end(ap);
}

// --- Fog+0x18950 ------------------------------------------------------------------------------

typedef int (*fog_isbad_fn)(const void* p, uint32_t size);
typedef uint32_t (*fog_strlen_fn)(const char* s);
typedef uint32_t (*fog_strcopy_fn)(char* dest, const char* src, uint32_t n);
typedef uint8_t* (*fog_listalloc_fn)(void* ctx, uint32_t size, uint32_t flags);

struct Fog18950Api {
    fog_isbad_fn is_bad_read_ptr;   // KERNEL32 IsBadReadPtr
    fog_strlen_fn sstrlen;          // Storm #506
    fog_strcopy_fn sstrcopy;        // Storm #501
    fog_listalloc_fn alloc;         // Fog+0x1290, (ctx, size, 0)
    void* alloc_ctx;                // &DAT_6ffa6ebc
};

// Fog module state, the addresses from the decompile.
static uint32_t g_fog_819a0;                       // DAT_6ff819a0
static uint32_t g_fog_818f8;                       // DAT_6ff818f8
static uint32_t g_fog_816f0;                       // DAT_6ff816f0, list count
static uint32_t g_fog_81848;                       // DAT_6ff81848, list bytes
static CRITICAL_SECTION g_fog_cs_a6e8c;            // DAT_6ffa6e8c
static uint32_t g_fog_head_a6ec0[4];               // DAT_6ffa6ec0 points here via g_fog_list
static uint8_t* g_fog_list = (uint8_t*)g_fog_head_a6ec0;

// 1.13c Fog+0x18950
// Record the caller-supplied blob on the debug/dump list. Gated by the two globals: does nothing
// unless logging is enabled (0x818f8) and not shutting down (0x819a0). A zero `skip` with an
// unreadable source forces skip to 1 (later fields then stay empty). The record is
// allocator-header + name + {data, len, field8, skip, line==0} + label + payload, the same layout
// Fog+0x14C70/0x10B80/0x11690 build. It is then unlinked from whatever list the recycled node sat
// on and pushed at the head of the global list, bumping the count and byte totals.
//
// Stack arguments (recovered from the disassembly, Ghidra mis-attributed two of them):
//   name, label, data, len, field8, skip, line.
extern "C"
void fog_sub_18950(const char* name, const char* label, const void* data, uint32_t len,
                   uint32_t field8, uint32_t skip, uint32_t line, const Fog18950Api* api) {
    if (g_fog_819a0 != 0 || g_fog_818f8 == 0) return;

    if (skip == 0 && api->is_bad_read_ptr(data, len) != 0) skip = 1;

    uint32_t size = api->sstrlen(name) + 0x15 + api->sstrlen(label) + 1;
    if (skip == 0) size += len;

    EnterCriticalSection(&g_fog_cs_a6e8c);
    uint8_t* block = api->alloc(api->alloc_ctx, size, 0);
    LeaveCriticalSection(&g_fog_cs_a6e8c);

    fog_st32(block + 0x08, size + 0x24);
    fog_st32(block + 0x0c, 2);
    fog_st32(block + 0x10, 1);
    GetLocalTime((LPSYSTEMTIME)(block + 0x14));

    uint32_t n = api->sstrcopy((char*)(block + 0x24), name, 0x7fffffff);
    uint8_t* q = block + 0x24 + n + 1;
    fog_st32(q + 0x00, (uint32_t)(uintptr_t)data);
    fog_st32(q + 0x04, len);
    fog_st32(q + 0x08, field8);
    fog_st32(q + 0x0c, skip);
    fog_st32(q + 0x10, (line == 0) ? 1u : 0u);

    uint8_t* r = q + 0x14;
    uint32_t n2 = api->sstrcopy((char*)r, label, 0x7fffffff);
    if (skip == 0) memcpy(r + n2 + 1, data, len);

    EnterCriticalSection(&g_fog_cs_a6e8c);
    const uint32_t next = fog_ld32(block);
    if (next != 0) {
        const uint32_t tag = fog_ld32(block + 4);
        uint8_t* slot;
        if ((int32_t)tag < 0) {
            slot = (uint8_t*)(uintptr_t)(~tag);
        } else {
            slot = block + (tag - fog_ld32((uint8_t*)(uintptr_t)next + 4));
        }
        fog_st32(slot, next);
        fog_st32((uint8_t*)(uintptr_t)next + 4, tag);
        fog_st32(block, 0);
        fog_st32(block + 4, 0);
    }
    uint8_t* root = g_fog_list;
    fog_st32(block, (uint32_t)(uintptr_t)root);
    fog_st32(block + 4, fog_ld32(root + 4));
    g_fog_816f0 += 1;
    fog_st32(root + 4, (uint32_t)(uintptr_t)block);
    g_fog_81848 += fog_ld32(block + 8);
    g_fog_list = block;
    LeaveCriticalSection(&g_fog_cs_a6e8c);
}

// --- Fog+0x16430 / Fog+0x16470 ----------------------------------------------------------------

struct FogAbortApi {
    fog_log_fn log;                 // Fog+0x190A0
    fog_fatal_fn fatal;             // Fog+0x10F30
    fog_thread_dump_fn dump_thread; // Fog+0x14C70
    fog_thread_dump_fn dump_stack;  // Fog+0x15C00
    fog_void_fn abort;              // Fog+0x184A0
    void* default_log;              // &DAT_6ff768ec
};

// 1.13c Fog+0x16430
// __fastcall, thread handle in ECX. Deadlock report: log the handle, raise a level-1 fatal at
// Fog/Src/Exceptions.cpp:0x2B4, dump that thread's stack, abort. Note there is no Fog+0x14C70
// call here, unlike the sibling below.
extern "C"
void fog_sub_16430(uint32_t thread_handle, const FogAbortApi* api) {
    api->log(api->default_log, "Dumping stack for thread handle: %08x", thread_handle);
    api->fatal(1, "..\\Source\\Fog\\Src\\Exceptions.cpp", 0x2b4, "Deadlock Detected");
    api->dump_stack(thread_handle);
    api->abort();
}

// 1.13c Fog+0x16470
// __fastcall, thread handle in ECX. Stack-dump report for one thread: log the handle, dump the
// thread context, dump its stack, abort. Both dumps take the handle.
extern "C"
void fog_sub_16470(uint32_t thread_handle, const FogAbortApi* api) {
    api->log(api->default_log, "Dumping stack for thread handle: %08x", thread_handle);
    api->dump_thread(thread_handle);
    api->dump_stack(thread_handle);
    api->abort();
}

// --- debug print path -------------------------------------------------------------------------

// 1.13c Fog+0x10DD0
// Format a varargs message into a 0x1000-byte buffer and hand it to OutputDebugStringA. The
// buffer is NUL-terminated both before (offset 0) and after formatting; the 0x1000 bound is what
// makes the trailing byte 0xFFF.
extern "C"
void fog_sub_10dd0(const char* fmt, ...) {
    char buf[0x1000];
    va_list ap;
    va_start(ap, fmt);
    buf[0] = '\0';
    std::vsnprintf(buf, 0x1000, fmt, ap);
    va_end(ap);
    buf[0xfff] = '\0';
    OutputDebugStringA(buf);
}

typedef uint16_t (*fog_async_key_fn)(int vkey);

// 1.13c Fog+0x10D60
// Same as Fog+0x10DD0, but only when GetAsyncKeyState(VK_CAPITAL=0x14) reports the key was
// pressed since the last poll (bit 0 of the 16-bit result). The env is first because the varargs
// list must stay last.
extern "C"
void fog_sub_10d60(fog_async_key_fn get_async_key_state, const char* fmt, ...) {
    if ((get_async_key_state(0x14) & 1) == 0) return;
    char buf[0x1000];
    va_list ap;
    va_start(ap, fmt);
    buf[0] = '\0';
    std::vsnprintf(buf, 0x1000, fmt, ap);
    va_end(ap);
    buf[0xfff] = '\0';
    OutputDebugStringA(buf);
}

// 1.13c Fog+0x10CD0
// Format into a 0x1000-byte buffer, debug-print it, then log it through Fog+0x18AF0 against the
// "LOG" tag with a "%s" format. The env is first because the varargs list must stay last.
extern "C"
void fog_sub_10cd0(fog_log_fn log, const char* fmt, ...) {
    char buf[0x1000];
    va_list ap;
    va_start(ap, fmt);
    buf[0] = '\0';
    std::vsnprintf(buf, 0x1000, fmt, ap);
    va_end(ap);
    buf[0xfff] = '\0';
    OutputDebugStringA(buf);
    log((void*)"LOG", "%s", buf);
}

typedef void (*fog_print_va_fn)(uint32_t flags, const char* fmt, va_list ap);

// 1.13c Fog+0x10D40
// Forward the (byte flags, format, ...) triple of the log macros to Fog+0x10BF0, which decides
// from the flag bits whether to prefix a timestamp, debug-print and/or log. The env is first
// because the varargs list must stay last.
extern "C"
void fog_sub_10d40(fog_print_va_fn print, uint32_t flags, const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    print(flags, fmt, ap);
    va_end(ap);
}

// --- Fog+0x1CDF0 / Fog+0x10640 ----------------------------------------------------------------

typedef uint8_t* (*fog_alloc5_fn)(uint32_t a, uint32_t b, uint32_t size, const char* file,
                                  uint32_t line);

struct FogAllocApi {
    fog_alloc5_fn alloc;   // Fog+0x1BDD0
    fog_fatal_fn fatal;    // Fog+0x10F30
    void* default_log;     // &DAT_6ff768ec
};

// 1.13c Fog+0x1CDF0
// __fastcall wrapper for the allocator, arguments (ECX, EDX, file, line) -> (0, ECX, EDX, file,
// line). A null allocation reports a level-2 fatal against the same file/line with the default
// log channel; either way the block is returned unchanged. `ret 0xc` (two stack arguments).
extern "C"
void* fog_sub_1cdf0(uint32_t ecx, uint32_t edx, const char* file, uint32_t line,
                    const FogAllocApi* api) {
    uint8_t* p = api->alloc(0, ecx, edx, file, line);
    if (p == nullptr) api->fatal(2, file, line, "");
    return p;
}

typedef void (*fog_validate_cs_fn)(CRITICAL_SECTION* cs);

// 1.13c Fog+0x10640
// __fastcall, critical section in ECX: validate the section (Fog+0x11690, which also carries the
// debug bookkeeping) and then leave it.
extern "C"
void fog_sub_10640(CRITICAL_SECTION* cs, fog_validate_cs_fn validate) {
    validate(cs);
    LeaveCriticalSection(cs);
}

// --- Fog+0x1C9E0 ------------------------------------------------------------------------------

typedef HANDLE (*fog_current_thread_fn)(void);
typedef int (*fog_thread_priority_fn)(HANDLE thread);
typedef int (*fog_set_thread_priority_fn)(HANDLE thread, int priority);
typedef int32_t (*fog_time32_fn)(int32_t* timer);
typedef void (*fog_read64_fn)(uint32_t* out, const uint32_t* src);
typedef void (*fog_write64_fn)(uint32_t* dst, const uint32_t* src);

struct FogClockApi {
    fog_current_thread_fn get_current_thread;      // KERNEL32 GetCurrentThread
    fog_thread_priority_fn get_thread_priority;    // KERNEL32 GetThreadPriority
    fog_set_thread_priority_fn set_thread_priority;// KERNEL32 SetThreadPriority
    fog_time32_fn time32;                          // CRT _time32
    fog_read64_fn read64;                          // Fog+0x1C9A0
    fog_write64_fn write64;                        // Fog+0x1C9C0
};

// DAT_6ff816d0, the 64-bit {seconds, tick} stamp shared with Fog+0x1C9A0/0x1C9C0.
static uint32_t g_fog_816d0[2];

// 1.13c Fog+0x1C9E0
// Uptime in seconds, periodically re-seeding the stamp and repairing the current thread's
// priority. Read the stamp; if the tick half is more than 30000 ms behind, re-assert the thread's
// current priority (a 0x7fffffff GetThreadPriority result means the thread is gone and is left
// alone) and store {_time32(0), GetTickCount()}. Elapsed whole seconds since the stamp are added
// to the stamp's seconds half. All arithmetic is unsigned, as in the retail divide-by-1000
// sequence (0x10624DD3).
extern "C"
int fog_sub_1c9e0(const FogClockApi* api) {
    const DWORD now = GetTickCount();
    uint32_t stamp[2];
    api->read64(stamp, g_fog_816d0);
    if ((uint32_t)(now - stamp[1]) > 30000u) {
        HANDLE thread = api->get_current_thread();
        const int priority = api->get_thread_priority(thread);
        if (priority != 0x7fffffff) {
            thread = api->get_current_thread();
            api->set_thread_priority(thread, priority);
        }
        uint32_t fresh[2];
        fresh[0] = (uint32_t)api->time32(nullptr);
        fresh[1] = (uint32_t)GetTickCount();
        api->write64(g_fog_816d0, fresh);
    }
    return (int)((uint32_t)(now - stamp[1]) / 1000u + stamp[0]);
}

// --- Fog+0x11160 ------------------------------------------------------------------------------

typedef void (*fog_write_header_fn)(uint32_t a, const void* b);
typedef void (*fog_exception_name_fn)(uint32_t code, char* buf, uint32_t size);
typedef void (*fog_format_address_fn)(uint32_t address, uint32_t size, char* buf,
                                      uint32_t* section, uint32_t* offset);
typedef void (*fog_dump_bytes_fn)(uint32_t esp, uint32_t eip);
typedef void (*fog_crash_file_fn)(uint32_t tag, uint32_t code, uint32_t address,
                                  uint32_t* extra);
typedef void (*fog_sstrprintf_fn)(char* buf, uint32_t size, const char* fmt, ...);
typedef int (*fog_message_box_fn)(void* hwnd, const char* text, const char* caption,
                                  uint32_t type);

struct Fog11160Api {
    fog_log_fn log;                        // Fog+0x190A0
    fog_write_header_fn write_header;      // Fog+0x10660
    fog_exception_name_fn exception_name;  // Fog+0x5FEB0
    fog_format_address_fn format_address;  // Fog+0x5FDB0
    fog_void_fn thread_info;               // Fog+0x10E30
    fog_dump_bytes_fn dump_bytes;          // Fog+0x10B80
    fog_crash_file_fn crash_file;          // Fog+0x14540
    fog_sstrprintf_fn sstrprintf;          // Storm #578
    fog_message_box_fn message_box;        // USER32 MessageBoxA
    fog_void_fn crash_hook;                // DAT_6ffa6e6c, may be null
    void* default_log;                     // &DAT_6ff768ec
};

static long g_fog_7fff0 = -1;   // DAT_6ff7fff0; .data+0x2FFF0 is 0xFFFFFFFF
static uint32_t g_fog_a6e70;    // DAT_6ffa6e70, "handling an exception"

// 1.13c Fog+0x11160
// Unhandled-exception filter. InterlockedIncrement on 0x6FF7FFF0 (initial -1) enters the handler
// only on the 0 -> 1 transition; the matching decrement runs on every path. The body sets the
// re-entrancy global, writes two blank lines, a rule and the crash header (Fog+0x10660), then
// logs the exception name and code, the fault address with section:offset, the five general
// registers, the five next registers, the six segment registers, the thread list and a code-byte
// dump around ESP/EIP, writes the crash file, calls the optional hook, and finally shows the
// classic "UNHANDLED EXCEPTION" MessageBox. Returns 1.
extern "C"
uint32_t fog_sub_11160(const void* exception_pointers, const Fog11160Api* api) {
    if (InterlockedIncrement(&g_fog_7fff0) == 0) {
        g_fog_a6e70 = 1;
        api->log(api->default_log, "");
        api->log(api->default_log, "");
        api->log(api->default_log, "------------------------------------------------------");
        api->write_header(0, (const void*)0x6ff5fd70);

        const uint8_t* ep = (const uint8_t*)exception_pointers;
        const uint8_t* rec = (const uint8_t*)(uintptr_t)fog_ld32(ep);
        const uint8_t* ctx = (const uint8_t*)(uintptr_t)fog_ld32(ep + 4);
        const uint32_t code = fog_ld32(rec);
        const uint32_t address = fog_ld32(rec + 0xc);

        char name[0x100];
        api->exception_name(code, name, 0x100);
        api->log(api->default_log, "***** UNHANDLED EXCEPTION: %s (%08x)", name, code);

        char where[0x104];
        uint32_t section = 0;
        uint32_t offset = 0;
        api->format_address(address, 0x104, where, &section, &offset);
        api->log(api->default_log, "Fault address:  %08X %02X:%08X %s", address, section, offset,
                 where);

        api->log(api->default_log, "eax:%08x ebx:%08x ecx:%08x edx:%08x esi:%08x",
                 fog_ld32(ctx + 0xb0), fog_ld32(ctx + 0xa4), fog_ld32(ctx + 0xac),
                 fog_ld32(ctx + 0xa8), fog_ld32(ctx + 0xa0));
        api->log(api->default_log, "edi:%08x ebp:%08x esp:%08x eip:%08x flg:%08x",
                 fog_ld32(ctx + 0x9c), fog_ld32(ctx + 0xb4), fog_ld32(ctx + 0xc4),
                 fog_ld32(ctx + 0xb8), fog_ld32(ctx + 0xc0));
        api->log(api->default_log, "cs:%04x ds:%04x es:%04x ss:%04x fs:%04x gs:%04x",
                 fog_ld32(ctx + 0xbc), fog_ld32(ctx + 0x98), fog_ld32(ctx + 0x94),
                 fog_ld32(ctx + 0xc8), fog_ld32(ctx + 0x90), fog_ld32(ctx + 0x8c));

        api->thread_info();
        api->dump_bytes(fog_ld32(ctx + 0xc4), fog_ld32(ctx + 0xb8));

        uint32_t extra = 0;
        api->crash_file(0x10d003c, code, address, &extra);
        if (api->crash_hook != nullptr) api->crash_hook();

        char caption[0x100];
        char text[0x1000];
        api->sstrprintf(caption, 0x100, "%s Exception", "Diablo II");
        api->sstrprintf(text, 0x1000, "UNHANDLED EXCEPTION:\n%s (%08x)", where, code);
        api->message_box(nullptr, text, caption, 0x30);
    }
    InterlockedDecrement(&g_fog_7fff0);
    return 1;
}
