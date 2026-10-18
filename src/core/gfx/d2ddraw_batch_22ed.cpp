// src/core/gfx/d2ddraw_batch_22ed.cpp -- twelve UNMAPPED 1.13c D2DDraw.dll rows reimplemented from the
// decompiles in build-wasm/ghidra-d2ddraw/decomp/0x<RVA>.c (alpha lane, no spec/vector yet). Retail
// base 0x6F8C0000; every RVA below is retail's and every module global is a static named at its
// retail RVA.
//
// All twelve are MSVC 2003 (VC7.1) Release CRT bodies the linker pulled into the DLL
//: the command-line/environment blocks (parse_cmdline, __setargv,
// ___crtGetEnvironmentStringsA), the heap bootstrap (___heap_select, __heap_init, __heap_term,
// ___sbh_heap_init), the exception filter (__XcptFilter), the stack probe (__chkstk) and the runtime
// message path (__FF_MSGBANNER, __NMSG_WRITE, __callnewh). Prior art: src/core/gfx/d2ddraw_batch_rowex.cpp
// (the same DLL's CRT state), src/core/d2client/d2client_batch_908f.cpp (__chkstk and the CRT leaves),
// src/core/realm/mcp_crt_mem.cpp and mcp_tiny_rowex.cpp (the seam-environment shape). Every kernel32
// call (GetModuleFileNameA, the environment block, HeapCreate/Alloc/Free, VirtualFree, WriteFile,
// MessageBoxA, UnhandledExceptionFilter) and the per-thread data block are host seams; the environment
// below is the one place a test runner installs them.
#include <cstdint>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <emscripten.h>

// --- retail module state -------------------------------------------------------------------------
// Heap bootstrap, the layout d2ddraw_batch_rowex.cpp's __heap_init seam points at.
static void*    g_heap_handle;          // +0x104DC DAT_6f8d04dc: HeapCreate handle
static uint32_t g_heap_type;            // +0x104E0 DAT_6f8d04e0: 1 = OS heap, 3 = small-block heap
static uint32_t g_os_platform = 2;      // +0x0EEE4 DAT_6f8ceee4: VER_PLATFORM_WIN32_NT
static uint32_t g_os_version = 5;       // +0x0EEF0 DAT_6f8ceef0: major version (NT5 = Win2k)
static void*    g_sbh_regions;          // +0x104C8 DAT_6f8d04c8: 0x140-byte region array
static uint32_t g_sbh_first;            // +0x104C0 DAT_6f8d04c0
static uint32_t g_sbh_count;            // +0x104C4 DAT_6f8d04c4: live regions
static void*    g_sbh_current;          // +0x104D0 DAT_6f8d04d0: region owning the last allocation
static uint32_t g_sbh_threshold;        // +0x104CC DAT_6f8d04cc: largest request routed to the sbh
static uint32_t g_sbh_seq = 0x10;       // +0x104D4 DAT_6f8d04d4: first free subregion
static char     g_heap_token;           // the port's opaque "process heap" (wasm has no Win32 heap)

// __setargv / command line.
static char     g_module_path[0x104];   // +0x0CF0A0 DAT_6f8cf0a0: GetModuleFileNameA buffer
static char*    g_pgmptr;               // +0x0EF14 DAT_6f8cef14: _pgmptr
static int32_t  g_argc;                 // +0x0EF3C? retail's writer is 0x6F8CEEF8
static char**   g_argv;                 // +0x0EF3C? retail's writer is 0x6F8CEEFC
static uint32_t g_setargv_static;       // +0x0F1A4 DAT_6f8cf1a4: cleared before the module-path read
static uint32_t g_mbctable_done;        // +0x10614 DAT_6f8d0614: ___initmbctable ran
// DAT_6f8d02a0 + byte + 1, bit 0x04: the lead-byte class of the current code page. The shipped image
// starts in the "C" locale, which has no lead bytes, so the table is all zero until a host installs one.
static uint8_t  g_mbctable[0x102];
static int32_t (*g_initmbctable)(void);  // ___initmbctable, a row not ported

// ___crtGetEnvironmentStringsA: 0 = untried, 1 = wide block available, 2 = ANSI block.
static uint32_t g_env_strings_kind;     // +0x0F1A8 DAT_6f8cf1a8

// __NMSG_WRITE message-mode words and the optional hook.
static uint32_t g_error_mode;           // +0x0EF34 DAT_6f8cef34: __error_mode
static uint32_t g_app_type;             // +0x0EF38 DAT_6f8cef38: __app_type
static void   (*g_msg_hook)(void);      // +0x0F1AC DAT_6f8cf1ac: _pfnmsg

// __XcptFilter: the per-thread exception-action table.
struct D2DDrawXcptAction { uint32_t num; uint32_t sig; uint32_t handler; };   // 0xC bytes
struct D2DDrawPtd {
    D2DDrawXcptAction* xcptacttab;      // Ghidra's _initaddr
    void*              initarg;         // Ghidra's _initarg: saved exception-pointer scratch
    uint32_t           sig_number;      // Ghidra's _pxcptacttab: the signal passed to a handler
};
static D2DDrawXcptAction g_xcpt_actions[0x40];
static D2DDrawPtd        g_ptd;         // __getptd (0x1BBE) is a seam until its row is ported
static uint32_t g_xcpt_first;           // +0x0E208 DAT_6f8ce208
static uint32_t g_xcpt_zero_count;      // +0x0E20C DAT_6f8ce20c
static uint32_t g_xcpt_table_len;       // +0x0E214 DAT_6f8ce214

// --- host seams ----------------------------------------------------------------------------------
// Every Win32 object above is installed here; the defaults are the wasm process's own stand-ins, so a
// row whose seams are not installed still follows its rule against zero/empty host state.
struct D2DDrawCrtEnv {
    const char* (*get_command_line)(void);                               // GetCommandLineA
    uint32_t    (*get_module_file_name)(char* buf, uint32_t size);       // GetModuleFileNameA
    void*       (*get_environment_strings_w)(void);                      // GetEnvironmentStringsW
    const char* (*get_environment_strings_a)(void);                      // GetEnvironmentStrings
    int32_t     (*wide_char_to_multi_byte)(uint32_t cp, const void* wide, int32_t wide_len,
                                           char* out, int32_t out_size); // WideCharToMultiByte
    uint32_t    (*get_last_error)(void);                                 // GetLastError
    void        (*free_environment_strings_w)(const void* block);        // FreeEnvironmentStringsW
    void        (*free_environment_strings_a)(const char* block);        // FreeEnvironmentStringsA
    void*       (*heap_create)(uint32_t options);                        // HeapCreate
    void        (*heap_destroy)(void* heap);                             // HeapDestroy
    void*       (*heap_alloc)(void* heap, uint32_t flags, uint32_t size);// HeapAlloc
    void        (*heap_free)(void* heap, uint32_t flags, void* block);   // HeapFree
    void        (*virtual_free)(void* addr, uint32_t size, uint32_t type);// VirtualFree
    int32_t     (*unhandled_exception_filter)(void* exception_ptrs);     // UnhandledExceptionFilter
    void        (*call_xcpt_handler)(uint32_t handler, uint32_t a, uint32_t b);
    int32_t     (*new_handler)(uint32_t size);                           // __callnewh's _pnh
    D2DDrawPtd* (*getptd)(void);                                         // 0x1BBE __getptd
    void        (*write_stderr)(const char* text, uint32_t len);         // WriteFile(STD_ERROR)
    int32_t     (*message_box)(const char* text, const char* caption, uint32_t type);
    const char* (*rt_message_text)(int32_t code);  // retail's 0x13-entry table holds hosted strings
};

static const char* env_default_command_line(void) { return ""; }
static uint32_t env_default_module_file_name(char* buf, uint32_t size) {
    if (buf != nullptr && size != 0) buf[0] = '\0';
    return 0;
}
static void* env_default_environment_w(void) { return nullptr; }
static const char* env_default_environment_a(void) { return nullptr; }
// CP_ACP stand-in: a byte per wide unit, '?' outside ASCII. Embedded NULs are preserved, which
// wcstombs would not do for a block of double-NUL-terminated entries.
static int32_t env_default_wide_to_mb(uint32_t, const void* wide, int32_t wide_len, char* out,
                                      int32_t out_size) {
    const wchar_t* in = (const wchar_t*)wide;
    if (out == nullptr) return wide_len;
    if (wide_len > out_size) return 0;
    for (int32_t i = 0; i < wide_len; ++i)
        out[i] = in[i] >= 0 && in[i] < 0x80 ? (char)in[i] : '?';
    return wide_len;
}
static uint32_t env_default_last_error(void) { return 0; }
static void env_default_free_w(const void*) {}
static void env_default_free_a(const char*) {}
static void* env_default_heap_create(uint32_t) { return &g_heap_token; }
static void env_default_heap_destroy(void*) {}
static void* env_default_heap_alloc(void*, uint32_t, uint32_t size) { return std::malloc(size); }
static void env_default_heap_free(void*, uint32_t, void* block) { std::free(block); }
static void env_default_virtual_free(void*, uint32_t, uint32_t) {}
static int32_t env_default_unhandled(void*) { return 1; }   // EXCEPTION_EXECUTE_HANDLER
static void env_default_call_handler(uint32_t, uint32_t, uint32_t) {}
static int32_t env_default_new_handler(uint32_t) { return 0; }
static D2DDrawPtd* env_default_getptd(void) { return &g_ptd; }
static void env_default_write_stderr(const char* text, uint32_t len) {
    if (text != nullptr) std::fwrite(text, 1, len, stderr);
}
static int32_t env_default_message_box(const char*, const char*, uint32_t) { return 0; }
static const char* env_default_rt_message(int32_t) { return nullptr; }

static D2DDrawCrtEnv g_env = {
    env_default_command_line,
    env_default_module_file_name,
    env_default_environment_w,
    env_default_environment_a,
    env_default_wide_to_mb,
    env_default_last_error,
    env_default_free_w,
    env_default_free_a,
    env_default_heap_create,
    env_default_heap_destroy,
    env_default_heap_alloc,
    env_default_heap_free,
    env_default_virtual_free,
    env_default_unhandled,
    env_default_call_handler,
    env_default_new_handler,
    env_default_getptd,
    env_default_write_stderr,
    env_default_message_box,
    env_default_rt_message,
};

// RVA 0x2A98, defined below; __heap_init reaches it on the small-block path.
int d2ddraw_sbh_heap_init_2a98(uint32_t threshold);

// The lead-byte class of a byte in the current code page: DAT_6f8d02a0[b + 1] & _M1 (0x04).
static bool d2ddraw_lead_byte(uint8_t b) { return (g_mbctable[(size_t)b + 1] & 0x04) != 0; }

// RVA 0x22ED: _parse_cmdline. Two-phase: the first call (argv and args NULL) sizes the argument area
// and the slot count; the second fills argv and the strings. Retail passes cmdstart/args/count in
// registers; the port takes them as arguments. Returns the character count (retail's ESI), stores the
// slot count including the final NULL slot in *slots.
static int d2ddraw_parse_cmdline(char** argv, int32_t* slots, const char* cmd, char* args) {
    const uint8_t* p = (const uint8_t*)cmd;
    uint8_t* out = (uint8_t*)args;
    int count = 0;
    bool in_quote = false;
    uint8_t c;

    *slots = 1;
    if (argv != nullptr) *argv++ = args;
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
            if (d2ddraw_lead_byte(c)) {          // a lead byte carries its trail byte along
                ++count;
                if (out != nullptr) *out++ = *p;
                ++p;
            }
            if (c == 0) {                       // the empty command line ends the first item
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
        if (argv != nullptr) *argv++ = (char*)out;
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
                    if (d2ddraw_lead_byte(c)) { ++p; ++count; }
                } else {
                    if (d2ddraw_lead_byte(c)) { *out++ = c; ++p; ++count; }
                    *out++ = *p;
                }
                ++count;
            }
            ++p;
        }
        if (out != nullptr) *out++ = 0;
        ++count;
    }
    if (argv != nullptr) *argv = nullptr;
    ++*slots;
    return count;
}

// RVA 0x2459: __setargv. Initialise the code-page table, publish the module path as _pgmptr, size the
// command line, allocate the argv array and the string area in one block (slots * 4 + char count),
// then fill both. Retail's command line arrives in a register from its CRT startup; the port reads it
// through the host seam.
static int d2ddraw_setargv(void) {
    if (g_mbctable_done == 0 && g_initmbctable != nullptr) g_initmbctable();
    g_setargv_static = 0;
    g_env.get_module_file_name(g_module_path, 0x104);
    g_pgmptr = g_module_path;
    const char* cmd = g_env.get_command_line();
    int32_t slots = 0;
    int chars = d2ddraw_parse_cmdline(nullptr, &slots, cmd, nullptr);
    char** argv = (char**)std::malloc((size_t)chars + (size_t)slots * 4);
    if (argv == nullptr) return -1;
    d2ddraw_parse_cmdline(argv, &slots, cmd, (char*)(argv + slots));
    g_argc = slots - 1;
    g_argv = argv;
    return 0;
}

// RVA 0x24FB: ___crtGetEnvironmentStringsA. Probe GetEnvironmentStringsW once (1 = wide), else an
// ERROR_CALL_NOT_IMPLEMENTED (0x78) makes it ANSI (2); other errors leave it untried. The chosen block
// is walked over its double-NUL terminator, copied into one malloc'd block and the source released.
static void* d2ddraw_crt_get_environment_strings_a(void) {
    const wchar_t* wide = nullptr;
    if (g_env_strings_kind == 0) {
        wide = (const wchar_t*)g_env.get_environment_strings_w();
        if (wide != nullptr) {
            g_env_strings_kind = 1;
        } else if (g_env.get_last_error() == 0x78) {
            g_env_strings_kind = 2;
        }
    }
    if (g_env_strings_kind != 1) {
        if (g_env_strings_kind != 2 && g_env_strings_kind != 0) return nullptr;
        const char* ansi = g_env.get_environment_strings_a();
        if (ansi == nullptr) return nullptr;
        const char* p = ansi;
        while (*p != 0) { while (*p != 0) ++p; ++p; }   // past this entry and its NUL
        size_t bytes = (size_t)(p - ansi) + 1;          // plus the final NUL
        void* dst = std::malloc(bytes);
        if (dst != nullptr) std::memcpy(dst, ansi, bytes);
        g_env.free_environment_strings_a(ansi);
        return dst;
    }
    if (wide == nullptr) {
        wide = (const wchar_t*)g_env.get_environment_strings_w();
        if (wide == nullptr) return nullptr;
    }
    const wchar_t* p = wide;
    while (*p != 0) { while (*p != 0) ++p; ++p; }
    int32_t chars = (int32_t)(p - wide) + 1;            // includes the final NUL
    int32_t bytes = g_env.wide_char_to_multi_byte(0, wide, chars, nullptr, 0);
    char* dst = bytes != 0 ? (char*)std::malloc((size_t)bytes) : nullptr;
    if (dst != nullptr && g_env.wide_char_to_multi_byte(0, wide, chars, dst, bytes) == 0) {
        std::free(dst);
        dst = nullptr;
    }
    g_env.free_environment_strings_w(wide);
    return dst;
}

// RVA 0x261D: ___heap_select. On NT5 and later the CRT uses the OS heap (1); otherwise the small-block
// heap (3). The two words come from GetVersion, zero in the shipped .data until the CRT startup set
// them; the port starts them at NT5 so the default path matches d2ddraw_batch_rowex.cpp's heap type 1.
static uint32_t d2ddraw_heap_select(void) {
    return (g_os_platform == 2 && g_os_version > 4) ? 1 : 3;
}

// RVA 0x2637: __heap_init(mtflag). Create the process heap (HEAP_NO_SERIALIZE when mtflag is 0), pick
// the heap kind, and on the small-block path also build its region array; a failure destroys the heap.
static int d2ddraw_heap_init(int32_t mtflag) {
    g_heap_handle = g_env.heap_create(mtflag == 0 ? 1u : 0u);
    if (g_heap_handle == nullptr) return 0;
    g_heap_type = d2ddraw_heap_select();
    if (g_heap_type == 3 && d2ddraw_sbh_heap_init_2a98(0x3F8) == 0) {
        g_env.heap_destroy(g_heap_handle);
        return 0;
    }
    return 1;
}

// RVA 0x2688: __heap_term. On the small-block path release every live region's reservation and its
// block header, then the region array; finally destroy the process heap.
static void d2ddraw_heap_term(void) {
    if (g_heap_type == 3) {
        uint8_t* region = (uint8_t*)g_sbh_regions;
        for (uint32_t i = 0; i < g_sbh_count; ++i, region += 0x14) {
            void* base = *(void**)(region + 0x0C);
            g_env.virtual_free(base, 0x100000, 0x4000);   // MEM_DECOMMIT
            g_env.virtual_free(base, 0, 0x8000);          // MEM_RELEASE
            g_env.heap_free(g_heap_handle, 0, *(void**)(region + 0x10));
        }
        g_env.heap_free(g_heap_handle, 0, g_sbh_regions);
    }
    g_env.heap_destroy(g_heap_handle);
}

// RVA 0x2710: __chkstk. Touch every 0x1000-byte page downward so the guard page faults before the
// frame is entered. wasm has no guard page, so the observable is the sequence of stored addresses.
static void d2ddraw_chkstk(uint32_t size, uint8_t* esp, uint32_t retaddr) {
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

// RVA 0x274D: __XcptFilter(xcptnum, exception_pointers). Find the action record for the code; no
// record (or a null handler) goes to UnhandledExceptionFilter, handler 5 disarms the record and answers
// 1, handler 1 answers -1. Any other handler is called with the record's signal word -- for signal 8
// the whole zero-count window is disarmed first and the exception code is mapped to the CRT signal
// number stored in the ptd. A null handler slot is not re-entered.
static int32_t d2ddraw_xcpt_filter(uint32_t xcptnum, void* exception_ptrs) {
    D2DDrawPtd* ptd = g_env.getptd();
    D2DDrawXcptAction* tab = ptd->xcptacttab;
    D2DDrawXcptAction* found = nullptr;
    if (tab != nullptr) {
        D2DDrawXcptAction* end = tab + g_xcpt_table_len;
        for (D2DDrawXcptAction* rec = tab; rec < end; ++rec) {
            if (rec->num == xcptnum) { found = rec; break; }
        }
    }
    if (found == nullptr || found->handler == 0) {
        return g_env.unhandled_exception_filter(exception_ptrs);
    }
    if (found->handler == 5) {
        found->handler = 0;
        return 1;
    }
    if (found->handler != 1) {
        void* saved_arg = ptd->initarg;
        ptd->initarg = exception_ptrs;
        if (found->sig == 8) {
            for (uint32_t i = g_xcpt_first; i < g_xcpt_first + g_xcpt_zero_count; ++i)
                g_xcpt_actions[i].handler = 0;
            uint32_t code = found->num;
            uint32_t saved_sig = ptd->sig_number;
            if (code == 0xC000008EU)      ptd->sig_number = 0x83;   // SIGFPE invalid
            else if (code == 0xC0000090U) ptd->sig_number = 0x81;   // SIGFPE
            else if (code == 0xC0000091U) ptd->sig_number = 0x84;   // overflow
            else if (code == 0xC0000093U) ptd->sig_number = 0x85;   // underflow
            else if (code == 0xC000008DU) ptd->sig_number = 0x82;   // SIGFPE zero divide
            else if (code == 0xC000008FU) ptd->sig_number = 0x86;   // inexact
            else if (code == 0xC0000092U) ptd->sig_number = 0x8A;   // stack fault
            g_env.call_xcpt_handler(found->handler, 8, ptd->sig_number);
            ptd->sig_number = saved_sig;
        } else {
            uint32_t handler = found->handler;   // retail loads the code address before disarming
            found->handler = 0;
            g_env.call_xcpt_handler(handler, found->sig, 0);
        }
        ptd->initarg = saved_arg;
    }
    return -1;
}

// RVA 0x28CC: _NMSG_WRITE(code). Look the code up in the 0x13-entry runtime-message table; no entry
// means nothing to print. With the message-mode words set the text goes to stderr; otherwise (except
// for 0xFC) retail builds "Runtime Error!\n\nProgram: <module>\n\n<text>" and shows the CRT message
// box. The message strings themselves are retail content, so the table is a host seam.
static void d2ddraw_nmsg_write(int32_t code) {
    const char* text = g_env.rt_message_text(code);
    if (text == nullptr) return;
    if (g_error_mode == 1 || (g_error_mode == 0 && g_app_type == 1)) {
        g_env.write_stderr(text, (uint32_t)std::strlen(text));
        return;
    }
    if (code == 0xFC) return;
    char caption[] = "Microsoft Visual C++ Runtime Library";
    const char* prefix = "Runtime Error!\n\nProgram: ";
    uint32_t name_len = g_env.get_module_file_name(g_module_path, 0x104);
    if (name_len == 0) g_module_path[0] = '\0';
    if (std::strlen(g_module_path) + 1 > 60) g_module_path[60 - 4] = '\0';   // retail truncates to 59
    char body[0x400];
    std::snprintf(body, sizeof body, "%s%s\n\n%s", prefix, g_module_path, text);
    g_env.message_box(body, caption, 0x12010);
}

// RVA 0x2A44: __FF_MSGBANNER. With the message-mode words set, print the banner (0xFC), run the
// installed hook, then print the float/version line (0xFF).
static void d2ddraw_ff_msgbanner(void) {
    if (!(g_error_mode == 1 || (g_error_mode == 0 && g_app_type == 1))) return;
    d2ddraw_nmsg_write(0xFC);
    if (g_msg_hook != nullptr) g_msg_hook();
    d2ddraw_nmsg_write(0xFF);
}

// RVA 0x2A7D: __callnewh(size). Ask the installed new-handler; 1 only when it returns non-zero.
static int d2ddraw_callnewh(uint32_t size) {
    if (g_env.new_handler == nullptr) return 0;
    return g_env.new_handler(size) != 0 ? 1 : 0;
}

// RVA 0x2A98: ___sbh_heap_init(threshold). One 0x140-byte block holds up to sixteen 0x14-byte region
// records; the bookkeeping words and the threshold are initialised for the first allocation.
int d2ddraw_sbh_heap_init_2a98(uint32_t threshold) {
    g_sbh_regions = g_env.heap_alloc(g_heap_handle, 0, 0x140);
    if (g_sbh_regions == nullptr) return 0;
    g_sbh_first = 0;
    g_sbh_count = 0;
    g_sbh_current = g_sbh_regions;
    g_sbh_threshold = threshold;
    g_sbh_seq = 0x10;
    return 1;
}

// Probe record ABI (the Fog batch lane's): arguments in retail order from r[0], scalar result in r[4].
extern "C" {

// 1.13c D2DDraw+0x022ED (parse_cmdline): r[0]=argv or 0, r[1]=slot-count out, r[2]=command line,
// r[3]=argument area or 0. r[4] = the character count retail keeps in ESI.
EMSCRIPTEN_KEEPALIVE void d2_fn_d2ddraw_22ed(uint32_t* r) {
    r[4] = (uint32_t)d2ddraw_parse_cmdline((char**)(uintptr_t)r[0], (int32_t*)(uintptr_t)r[1],
                                           (const char*)(uintptr_t)r[2], (char*)(uintptr_t)r[3]);
}

// 1.13c D2DDraw+0x02459 (__setargv): no arguments; r[4] = 0 on success, -1 when the block cannot be
// allocated.
EMSCRIPTEN_KEEPALIVE void d2_fn_d2ddraw_2459(uint32_t* r) {
    r[4] = (uint32_t)d2ddraw_setargv();
}

// 1.13c D2DDraw+0x024FB (___crtGetEnvironmentStringsA): r[4] = the copied block, or 0.
EMSCRIPTEN_KEEPALIVE void d2_fn_d2ddraw_24fb(uint32_t* r) {
    r[4] = (uint32_t)(uintptr_t)d2ddraw_crt_get_environment_strings_a();
}

// 1.13c D2DDraw+0x0261D (___heap_select): r[4] = 1 (OS heap) or 3 (small-block heap).
EMSCRIPTEN_KEEPALIVE void d2_fn_d2ddraw_261d(uint32_t* r) {
    r[4] = d2ddraw_heap_select();
}

// 1.13c D2DDraw+0x02637 (__heap_init): r[0] = mtflag; r[4] = 1 on success, 0 on failure.
EMSCRIPTEN_KEEPALIVE void d2_fn_d2ddraw_2637(uint32_t* r) {
    r[4] = (uint32_t)d2ddraw_heap_init((int32_t)r[0]);
}

// 1.13c D2DDraw+0x02688 (__heap_term): no arguments, no result.
EMSCRIPTEN_KEEPALIVE void d2_fn_d2ddraw_2688(uint32_t* r) {
    (void)r;
    d2ddraw_heap_term();
}

// 1.13c D2DDraw+0x02710 (__chkstk): r[0] = size in EAX, r[1] = entry ESP, r[2] = the probed return
// address.
EMSCRIPTEN_KEEPALIVE void d2_fn_d2ddraw_2710(uint32_t* r) {
    d2ddraw_chkstk(r[0], (uint8_t*)(uintptr_t)r[1], r[2]);
}

// 1.13c D2DDraw+0x0274D (__XcptFilter): r[0] = exception code, r[1] = EXCEPTION_POINTERS; r[4] = the
// filter result (-1, 1, or UnhandledExceptionFilter's answer).
EMSCRIPTEN_KEEPALIVE void d2_fn_d2ddraw_274d(uint32_t* r) {
    r[4] = (uint32_t)d2ddraw_xcpt_filter(r[0], (void*)(uintptr_t)r[1]);
}

// 1.13c D2DDraw+0x028CC (__NMSG_WRITE): r[0] = the runtime message code.
EMSCRIPTEN_KEEPALIVE void d2_fn_d2ddraw_28cc(uint32_t* r) {
    d2ddraw_nmsg_write((int32_t)r[0]);
}

// 1.13c D2DDraw+0x02A44 (__FF_MSGBANNER): no arguments, no result.
EMSCRIPTEN_KEEPALIVE void d2_fn_d2ddraw_2a44(uint32_t* r) {
    (void)r;
    d2ddraw_ff_msgbanner();
}

// 1.13c D2DDraw+0x02A7D (__callnewh): r[0] = size; r[4] = 1 when the new-handler freed room, else 0.
EMSCRIPTEN_KEEPALIVE void d2_fn_d2ddraw_2a7d(uint32_t* r) {
    r[4] = (uint32_t)d2ddraw_callnewh(r[0]);
}

// 1.13c D2DDraw+0x02A98 (___sbh_heap_init): r[0] = threshold; r[4] = 1 on success, 0 on failure.
EMSCRIPTEN_KEEPALIVE void d2_fn_d2ddraw_2a98(uint32_t* r) {
    r[4] = (uint32_t)d2ddraw_sbh_heap_init_2a98(r[0]);
}

// Scenario setup: install the host seams (null restores nothing; the defaults stay as initialised
// above), and pin the message-mode words/exception table the rows read.
EMSCRIPTEN_KEEPALIVE void d2_fn_d2ddraw_env(const D2DDrawCrtEnv* env) {
    if (env != nullptr) g_env = *env;
}
EMSCRIPTEN_KEEPALIVE void d2_fn_d2ddraw_28cc_pin(uint32_t error_mode, uint32_t app_type,
                                                 void (*hook)(void)) {
    g_error_mode = error_mode;
    g_app_type = app_type;
    g_msg_hook = hook;
}
EMSCRIPTEN_KEEPALIVE void d2_fn_d2ddraw_274d_pin(uint32_t table_len, uint32_t first,
                                                 uint32_t zero_count) {
    g_xcpt_table_len = table_len;
    g_xcpt_first = first;
    g_xcpt_zero_count = zero_count;
    g_ptd.xcptacttab = g_xcpt_actions;
}
EMSCRIPTEN_KEEPALIVE void d2_fn_d2ddraw_261d_pin(uint32_t platform, uint32_t version) {
    g_os_platform = platform;
    g_os_version = version;
}

}  // extern "C"
