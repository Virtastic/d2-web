// src/core/storm/storm_batch_3c2b0.cpp -- twelve 1.13c Storm.dll rows reimplemented from the cached
// decompiles (build-wasm/ghidra-storm/decomp/0x<RVA>.c): the SReg value API (SRegLoadData 0x3C2B0,
// SRegLoadValue 0x3C1D0, SRegSaveData 0x3C150, SRegSaveString 0x3C0E0, SRegSaveValue 0x3C080,
// SRegDeleteValue 0x3C3E0, ordinals 422-426/428) and the SErr/SVid half (SErrSetLastError 0x2F130,
// SErrRegisterMessageSource 0x2F250, SErrGetErrorStr 0x2F3F0, SErrReportNamedResourceLeak 0x2FB30,
// SErrDisplayError 0x2F590, SVidDestroy 0x23250, SVidPlayEnd 0x230C0, ordinals 451/458/461-466).
// Stage A batch lane (OBJECTIVE v8.3): implementation + marker only, no spec, no vector.
//
// Every body is the literal retail body. Retail globals are file statics tagged with their 1.13c
// address; unmapped internal callees are seams on StormBatch3c2b0Env in retail argument order, and
// every body takes that env AFTER its retail argument list. The kernel32/user32 calls the compat
// shim does not carry (heap, file find, message box, module handles) are local seams too, so the
// translation unit is self-contained. One decompiler correction is called out at its site: 0x3C2B0
// reuses its valuename stack slot for the callback's length out-value.
#include <cstdint>
#include <cstring>
#include <cstdarg>
#include <cstdlib>
#include <strings.h>
#include <Windows.h>
#include "storm_state.h"
extern CRITICAL_SECTION g_stormErrLock;

// ---- retail module state (all zero-initialised unless noted) -----------------------------------
namespace {

uint32_t g_fatalActive;         // 0x6FC45384  a fatal error has terminated the process
uint32_t g_inError;             // 0x6FC42FF8  SErrDisplayError re-entry guard
uint32_t g_hwnd;                // 0x6FC42FD0  owner window for the error box
uint32_t g_notifyList;          // 0x6FC45960  message-source notify list
uint32_t g_debugMem;            // 0x6FC45A70  "Internal"/"Debug Memory" registry flag
uint32_t g_debugMemInit;        // 0x6FC45A74  flag read once
uint32_t g_flag43138;           // 0x6FC43138  debugger attached -> debug output, no dialog
uint32_t g_svidCtx;             // 0x6FC4539C  SVid context (three owned objects)
uint32_t g_svidAlloc;           // 0x6FC453A0  SVid allocator vtable (function-pointer array)
uint32_t g_svidDll;             // 0x6FC453A4  loaded video-codec DLL
uint32_t g_videoList;           // 0x6FC459F0  open video list head (next at +4)

constexpr const char* kSvidCpp = "..\\3rdParty\\STORM\\SOURCE\\SVID.CPP";
constexpr const char* kEmpty = "";

void StormSetLastError(uint32_t e) { g_stormLastError = (int32_t)e; }
void set_storm_error(uint32_t e) { g_stormLastError = (int32_t)e; StormSetLastError(e); }
void FreeLibrary(uint32_t) {}
void CloseHandle(uint32_t) {}

uint32_t rd32(const void* p) { uint32_t v; std::memcpy(&v, p, 4); return v; }
void wr32(void* p, uint32_t v) { std::memcpy(p, &v, 4); }
uint8_t* at(uint32_t p, uintptr_t off) { return (uint8_t*)(uintptr_t)(p + off); }

// variable-argument host seams: a plain formatted write / bounded formatted write
int wsprintf(char* dst, const char* fmt, ...) {
    va_list a; va_start(a, fmt); int r = vsprintf(dst, fmt, a); va_end(a); return r;
}
int snprintf_bounded(char* dst, uint32_t size, const char* fmt, ...) {
    va_list a; va_start(a, fmt); int r = vsnprintf(dst, size, fmt, a); va_end(a); return r;
}

}  // namespace

// ---- unmapped internal callees and Win32 calls the compat shim does not carry -------------------
struct StormBatch3c2b0Env {
    // registry core
    uint32_t (*f_3bdd0)(const char* key, const char* name, uint8_t flags, uint32_t* type,
                        uint8_t* data, uint32_t size, uint32_t* length);   // 0x3BDD0 query raw
    uint32_t (*f_3bcb0)(const char* key, const char* name, uint32_t type, const void* data,
                        uint32_t size);                                    // 0x3BCB0 save raw
    uint32_t (*f_3bf30)(const char* key, const char* name);                // 0x3BF30 delete key
    // error subsystem
    void     (*f_2f180)(void);                                             // 0x2F180 init+enter lock
    void     (*f_2f200)(uint32_t id);                                      // 0x2F200 register source
    uint32_t (*f_2f3f0)(uint32_t msgId, char* buf, uint32_t count);        // 0x2F3F0 (this file)
    uint32_t (*f_2f590)(uint32_t code, const char* file, int32_t line, const char* desc,
                        int32_t recoverable, uint32_t exitcode);           // 0x2F590 (this file)
    const char* (*f_2f1c0)(int32_t index);                                 // 0x2F1C0 STRINGTABLE 0x5100+i
    uint32_t (*f_2f330)(const char* src, char* dst, uint32_t size);        // 0x2F330 symbol rewrite
    void     (*f_349e0)(uint32_t);                                         // 0x349E0 flush log streams
    // string helpers
    uint32_t (*f_2afd0)(const char* s);                                    // 0x2AFD0 strlen
    uint32_t (*f_2b080)(char* dst, const char* src, uint32_t size);        // 0x2B080 bounded strcpy
    char*    (*f_2b280)(const char* s, char c);                            // Storm+0x3B280 (Storm_Sub3B280) last occurrence
    int      (*f_2ba80)(char* dst, uint32_t size, const char* fmt, ...);   // 0x2BA80 bounded format
    int      (*f_wsprintf)(char* dst, const char* fmt, ...);               // wsprintfA
    int      (*f_2bf30_load)(uint32_t mod, uint32_t id, char* buf, uint32_t n); // LoadStringA
    uint32_t (*format_message)(uint32_t flags, const void* source, uint32_t msgId, uint32_t lang,
                               char* buf, uint32_t n);                       // FormatMessageA
    void     (*f_b960)(void* p, const char* file, int32_t line);           // 0xB960 SMemFree
    void     (*f_45a0)(uint32_t listHead);                                 // 0x45A0 AUVIDEOREC unlink
    // Win32
    void*    (*heap_alloc)(size_t);                                        // HeapAlloc/GetProcessHeap
    uint32_t (*get_module_file_name)(uint32_t mod, char* buf, uint32_t n); // GetModuleFileNameA
    void*    (*find_first)(const char* path, uint8_t* find_data);          // FindFirstFileA
    void     (*find_close)(void* h);
    int      (*message_box)(void* hwnd, const char* text, const char* caption, uint32_t type);
    int      (*is_window)(void* hwnd);
    int      (*is_window_visible)(void* hwnd);
    void*    (*get_current_process)(void);
    int      (*get_exit_code_process)(void* proc, uint32_t* code);
    void     (*terminate_process)(void* proc, uint32_t code);
    uint32_t (*get_version)(void);
};

namespace {

// retail's _WIN32_FIND_DATAA.cFileName lives at +0x2C (alternate name 0x0C, cFileName 0x2C)
const char* find_cfilename(const uint8_t* fd) { return (const char*)(fd + 0x2C); }

const char* empty_str() { return kEmpty; }

}  // namespace

// =================================================================================================
// SReg value API (ordinals 422-426, 428)
// =================================================================================================

// 1.13c Storm+0x3C2B0: SRegLoadData. Validate key/value/data/size (error 0x57 on any miss), read the
// value through 0x3BDD0, then terminate a REG_SZ at min(length, size-1) or reformat a REG_DWORD in
// place as "%u". Ghidra aliases the callback length out into the valuename stack slot, so the length
// is an explicit out parameter of the seam here. Always 1 on success.
extern "C"
uint32_t Storm_SRegLoadData(StormBatch3c2b0Env& e, const char* key, const char* name, uint32_t flags,
                            uint8_t* data, uint32_t size) {
    if (key == nullptr || *key == '\0' || name == nullptr || *name == '\0' ||
        data == nullptr || size == 0) {
        set_storm_error(0x57);
        return 0;
    }
    uint32_t type = 0, length = 0;
    if (e.f_3bdd0(key, name, (uint8_t)flags, &type, data, size, &length) == 0) return 0;
    if (type == 1) {
        uint32_t end = length <= size - 1 ? length : size - 1;
        data[end] = '\0';
    } else if (type == 4) {
        e.f_wsprintf((char*)data, "%u", rd32(data));
    }
    return 1;
}

// 1.13c Storm+0x3C1D0: SRegLoadValue. Same validation, read into a 0x100-byte scratch through
// 0x3BDD0; a REG_SZ converts with strtoul(base 0), a REG_DWORD is copied verbatim into *value.
extern "C"
uint32_t Storm_SRegLoadValue(StormBatch3c2b0Env& e, const char* key, const char* name, uint8_t flags,
                             uint32_t* value) {
    if (key == nullptr || *key == '\0' || name == nullptr || *name == '\0' || value == nullptr) {
        set_storm_error(0x57);
        return 0;
    }
    char buf[0x100];
    buf[0] = '\0';
    uint32_t type = 0, length = 0;
    if (e.f_3bdd0(key, name, flags, &type, (uint8_t*)buf, 0x100, &length) == 0) return 0;
    if (type == 1) {
        *value = (uint32_t)strtoul(buf, nullptr, 0);
    } else if (type == 4) {
        *value = rd32(buf);
    }
    return 1;
}

// 1.13c Storm+0x3C150: SRegSaveData. Key/value must be non-empty; a null data pointer becomes the
// empty string. The stored type is ((flags & 0x80) | 0x60) >> 5: 0x60>>5 = 3 (REG_BINARY), 0xE0>>5
// = 7 when bit 7 is set. No size check, as retail.
extern "C"
uint32_t Storm_SRegSaveData(StormBatch3c2b0Env& e, const char* key, const char* name, uint32_t flags,
                            const uint8_t* data, uint32_t size) {
    if (key == nullptr || *key == '\0' || name == nullptr || *name == '\0') {
        set_storm_error(0x57);
        return 0;
    }
    if (data == nullptr) data = (const uint8_t*)kEmpty;
    uint32_t type = ((flags & 0x80) | 0x60) >> 5;
    return e.f_3bcb0(key, name, type, data, size);
}

// 1.13c Storm+0x3C0E0: SRegSaveString. Validate key/value/string, then save as REG_SZ with
// strlen(string)+1 bytes.
extern "C"
uint32_t Storm_SRegSaveString(StormBatch3c2b0Env& e, const char* key, const char* name,
                              const char* string) {
    if (key == nullptr || *key == '\0' || name == nullptr || *name == '\0' || string == nullptr) {
        set_storm_error(0x57);
        return 0;
    }
    return e.f_3bcb0(key, name, 1, string, e.f_2afd0(string) + 1);
}

// 1.13c Storm+0x3C080: SRegSaveValue. Save the caller's DWORD slot as REG_DWORD, 4 bytes.
extern "C"
uint32_t Storm_SRegSaveValue(StormBatch3c2b0Env& e, const char* key, const char* name,
                             uint32_t value) {
    if (key == nullptr || *key == '\0' || name == nullptr || *name == '\0') {
        set_storm_error(0x57);
        return 0;
    }
    return e.f_3bcb0(key, name, 4, &value, 4);
}

// 1.13c Storm+0x3C3E0: SRegDeleteValue. Validate key/value, then delete the key through 0x3BF30.
extern "C"
uint32_t Storm_SRegDeleteValue(StormBatch3c2b0Env& e, const char* key, const char* name) {
    if (key == nullptr || *key == '\0' || name == nullptr || *name == '\0') {
        set_storm_error(0x57);
        return 0;
    }
    return e.f_3bf30(key, name);
}

// =================================================================================================
// SErr / SVid (ordinals 451, 458, 461-466)
// =================================================================================================

// 1.13c Storm+0x2F130: SErrSetLastError. Store the code at 0x6FC45388 and tail-call SetLastError.
extern "C"
void Storm_SErrSetLastError(uint32_t code) {
    g_stormLastError = (int32_t)code;
    StormSetLastError(code);
}

// 1.13c Storm+0x2F250: SErrRegisterMessageSource. Lazy-init the SErr lock, allocate a 12-byte node
// {u16 id; u16 pad; void* source at +4; node* next at +8}, push it on 0x6FC4537C. Always 1.
extern "C"
uint32_t Storm_SErrRegisterMessageSource(StormBatch3c2b0Env& e, uint16_t id, uint32_t source) {
    if (InterlockedIncrement((long volatile*)&g_stormLockInit) == 0) {
        InitializeCriticalSection(&g_stormErrLock);
    } else {
        InterlockedDecrement((long volatile*)&g_stormLockInit);
    }
    EnterCriticalSection(&g_stormErrLock);
    auto* node = (StormSrcNode*)e.heap_alloc(sizeof(StormSrcNode));
    std::memset(node, 0, sizeof *node);
    node->id = id;
    node->source = (void*)(uintptr_t)source;
    node->next = g_stormSrcList;
    g_stormSrcList = node;
    LeaveCriticalSection(&g_stormErrLock);
    return 1;
}

// 1.13c Storm+0x2F3F0: SErrGetErrorStr. Validate buffer/size (0x57), enter the SErr lock, register
// the three built-in sources once, walk 0x6FC4537C for a node whose u16 id equals
// (msgId >> 16) & 0xfff, then FormatMessageA with FROM_HMODULE (0x800) when a source matched and
// FROM_SYSTEM (0x1000) otherwise. The buffer is cleared before the call either way.
extern "C"
uint32_t Storm_SErrGetErrorStr(StormBatch3c2b0Env&, uint32_t msgId, char* buf, uint32_t count) {
    return Storm_SErrGetErrorStr_2F3F0(msgId, buf, count);  // real body: storm_serr.cpp
}

// 1.13c Storm+0x2FB30: SErrReportNamedResourceLeak. Read the debug-memory registry flag once; when
// set, format "type (name)" and either raise a fatal 0x8510007F error through SErrDisplayError or,
// with a debugger present, write "Storm Error : handle never released -- ..." to the debug output.
extern "C"
void Storm_SErrReportNamedResourceLeak(StormBatch3c2b0Env& e, const char* type, const char* name) {
    if (g_debugMemInit == 0) {
        g_debugMemInit = 1;
        Storm_SRegLoadValue(e, "Internal", "Debug Memory", 0, &g_debugMem);
    }
    if (g_debugMem == 0) return;
    char what[0x100];
    if (name == nullptr || *name == '\0') {
        e.f_2ba80(what, 0x100, "%s", type);
    } else {
        e.f_2ba80(what, 0x100, "%s (%s)", type, name);
    }
    if (g_flag43138 == 0) {
        e.f_2f590(0x8510007f, what, -3, nullptr, 1, 1);
        return;
    }
    char line[200];
    e.f_wsprintf(line, "Storm Error : handle never released -- %s\n", what);
    OutputDebugStringA(line);
}

// 1.13c Storm+0x2F590: SErrDisplayError. The fatal-error dialog: re-entry guard, flush the log
// streams, compose the "This application has encountered an error" body from the module's
// STRINGTABLE (0x5101/0x5102) with the error text, source location and description, give every
// registered notify callback a chance to veto the box, then show it. A recoverable error returns 1
// on IDNO, any other answer kills the process with exitcode (an IDCANCEL answers __break()).
extern "C"
uint32_t Storm_SErrDisplayError(StormBatch3c2b0Env& e, uint32_t errorcode, const char* filename,
                                int32_t linenumber, const char* description, int32_t recoverable,
                                uint32_t exitcode) {
    if (g_fatalActive != 0 || g_inError != 0) return 0;
    g_inError = 1;
    e.f_2f180();
    e.f_349e0(0);
    char modulePath[0x104];
    char program[0x104];
    char msg[0x100];
    modulePath[0] = '\0';
    program[0] = '\0';
    msg[0] = '\0';
    e.get_module_file_name(0, modulePath, 0x104);
    uint8_t findData[0x140];
    std::memset(findData, 0, sizeof findData);
    void* fh = e.find_first(modulePath, findData);
    if (fh != nullptr) e.find_close(fh);
    e.f_2b080(program, find_cfilename(findData), 0x104);
    char* dot = e.f_2b280(program, '.');
    if (dot != nullptr) *dot = '\0';
    Storm_SErrGetErrorStr(e, errorcode, msg, 0x100);
    if (msg[0] == '\0') {
        e.f_wsprintf(msg, e.f_2f1c0(0), errorcode & 0xffff, errorcode);
    }
    const char* fileArg = filename;
    char rewritten[0x100];
    if (filename != nullptr && *filename != '\0' &&
        (linenumber == -2 || linenumber == -3)) {
        if (e.f_2f330(filename, rewritten, 0x100) != 0) fileArg = rewritten;
    }
    char detail[0x408];
    char* p = detail;
    if (fileArg != nullptr) p += e.f_2b080(p, fileArg, 0x7fffffff);
    if (linenumber > 0) {
        e.f_wsprintf(p, "(%u)", (uint32_t)linenumber);
        p += e.f_2afd0(p);
    }
    e.f_wsprintf(p, " : error %u: ", errorcode & 0xffff);
    p += e.f_2afd0(p);
    e.f_2b080(p, msg, 0x7fffffff);
    OutputDebugStringA(detail);

    char body[0x408];
    char tmp[0x100];
    int n = e.f_2bf30_load(g_stormMod, 0x5101, tmp, 0x100);
    const char* fmt = n != 0 ? tmp : e.f_2f1c0(1);
    e.f_wsprintf(body, fmt, msg);
    p = body + e.f_2afd0(body);
    n = e.f_2bf30_load(g_stormMod, 0x5102, tmp, 0x100);
    fmt = n != 0 ? tmp : e.f_2f1c0(2);
    e.f_wsprintf(p, fmt, modulePath);
    p += e.f_2afd0(p);
    // Index per the 1.13c jump table at 0x2F9F0 and the mov-esi sites: -4->11, -3->6, -2->5, -1->4
    // (one argument each), any other line->3 ("File:\t%s\nLine:\t%d\n", two arguments).
    if (fileArg != nullptr && *fileArg != '\0') {
        switch (linenumber) {
        case -4: e.f_wsprintf(p, e.f_2f1c0(11), fileArg); break;
        case -3: e.f_wsprintf(p, e.f_2f1c0(6), fileArg); break;
        case -2: e.f_wsprintf(p, e.f_2f1c0(5), fileArg); break;
        case -1: e.f_wsprintf(p, e.f_2f1c0(4), fileArg); break;
        default: e.f_wsprintf(p, e.f_2f1c0(3), fileArg, linenumber);
        }
        p += e.f_2afd0(p);
    }
    // Description label: 7 for the 0x85100000 code, 8 otherwise (1.13c 0x2F875..0x2F89F).
    const char* desc = description != nullptr ? description : empty_str();
    e.f_wsprintf(p, e.f_2f1c0(errorcode == 0x85100000u ? 7 : 8), desc);
    p += e.f_2afd0(p);
    // Trailer copied, not formatted: 10 when recoverable, else 9 (1.13c 0x2F8B9..0x2F8D1).
    e.f_2b080(p, e.f_2f1c0(recoverable != 0 ? 10 : 9), 0x7fffffff);

    int rc = 7;
    void* hwnd = (void*)(uintptr_t)g_hwnd;
    uint32_t node = (int32_t)g_notifyList < 1 ? 0 : g_notifyList;
    for (; (int32_t)node > 0; node = rd32(at(node, 4))) {
        const char* d = description != nullptr ? description : empty_str();
        typedef int32_t (*NotifyFn)(uint32_t, char*, const char*, int32_t, const char*);
        NotifyFn fn = (NotifyFn)(uintptr_t)rd32(at(node, 8));
        if (fn(errorcode, msg, fileArg, linenumber, d) == 0) goto left_lock;
        hwnd = (void*)(uintptr_t)g_hwnd;
    }
    if (e.is_window(hwnd) == 0 || e.is_window_visible(hwnd) == 0) hwnd = nullptr;
    rc = e.message_box(hwnd, body, program,
                       (uint32_t)((recoverable != 0) * 2 + 1) | 0x52010);
left_lock:
    LeaveCriticalSection(&g_stormErrLock);
    if (recoverable != 0 && rc == 7) {
        g_inError = 0;
        return 1;
    }
    if (rc == 2) {
        abort();  // 0x2F1F0 __break, does not return
    }
    g_fatalActive = 1;
    uint32_t ver = e.get_version();
    if ((int32_t)ver < 0) {
        uint32_t code = 0;
        void* proc = e.get_current_process();
        if (e.get_exit_code_process(proc, &code) != 0 && code != 0x103) {
            g_inError = 0;
            return 0;
        }
    }
    e.terminate_process(e.get_current_process(), exitcode);
    g_inError = 0;
    return 0;
}

// 1.13c Storm+0x230C0: SVidPlayEnd. Validate and find the video in 0x6FC459F0; release the two
// objects behind +0x58 through the allocator vtable (slots 0 and 3), free the +0x58 record (line
// 0x406), close +0x08 (reset to -1), free +0x4C (clearing +0x2C) at line 0x40F and +0xA0 at line
// 0x415, then unlink the AUVIDEOREC record at 0x6FC459E8. Returns 1 only when the video was found
// and had a +0x58 record.
extern "C"
uint32_t Storm_SVidPlayEnd(StormBatch3c2b0Env& e, uint8_t* video) {
    if (video == nullptr) {
        set_storm_error(0x57);
        return 0;
    }
    // 0x22FA0: is video in the 0x6FC459F0 list (next at +4)?
    uint32_t p = g_videoList;
    if ((int32_t)p < 1) p = 0;
    bool found = false;
    while ((int32_t)p >= 1) {
        if ((uint8_t*)(uintptr_t)p == video) { found = true; break; }
        p = rd32(at(p, 4));
    }
    if (!found) return 0;
    if (rd32(video + 0x58) == 0) return 0;
    uint32_t rec = rd32(video + 0x58);
    uint32_t obj0 = rd32(at(rec, 0));
    uint32_t obj1 = rd32(at(rec, 4));
    uint32_t alloc = g_svidAlloc;
    if (obj0 != 0) {
        typedef void (*Release0)(uint32_t);
        ((Release0)(uintptr_t)rd32(at(alloc, 0)))(obj0);
    }
    if (obj1 != 0) {
        typedef void (*Release3)(uint32_t);
        ((Release3)(uintptr_t)rd32(at(alloc, 0xC)))(obj1);
    }
    e.f_b960((void*)(uintptr_t)rec, kSvidCpp, 0x406);
    wr32(video + 0x58, 0);
    if (rd32(video + 0x08) != 0xFFFFFFFFu) {
        CloseHandle(rd32(video + 0x08));
        wr32(video + 0x08, 0xFFFFFFFFu);
    }
    if (rd32(video + 0x4C) != 0) {
        e.f_b960((void*)(uintptr_t)rd32(video + 0x4C), kSvidCpp, 0x40F);
        wr32(video + 0x4C, 0);
        wr32(video + 0x2C, 0);
    }
    if (rd32(video + 0xA0) != 0) {
        e.f_b960((void*)(uintptr_t)rd32(video + 0xA0), kSvidCpp, 0x415);
    }
    // 0x45A0 unlinks and frees the current AUVIDEOREC from the list at 0x6FC459E8.
    e.f_45a0(0x6FC459E8u);
    return 1;
}

// 1.13c Storm+0x23250: SVidDestroy. While 0x6FC459F0 holds a video, report it as a leaked HSVIDEO
// handle and close it through SVidPlayEnd. Then free the three context members and the context,
// the allocator object, and unload the codec DLL. Always 1.
extern "C"
uint32_t Storm_SVidDestroy(StormBatch3c2b0Env& e) {
    while ((int32_t)g_videoList > 0) {
        Storm_SErrReportNamedResourceLeak(e, "HSVIDEO", nullptr);
        Storm_SVidPlayEnd(e, (uint8_t*)(uintptr_t)g_videoList);
    }
    if (g_svidCtx != 0) {
        void* p = (void*)(uintptr_t)g_svidCtx;
        void* owned[3];
        std::memcpy(owned, p, sizeof owned);
        for (int i = 0; i < 3; ++i) std::free(owned[i]);
        std::free(p);
        g_svidCtx = 0;
    }
    if (g_svidAlloc != 0) {
        std::free((void*)(uintptr_t)g_svidAlloc);
        g_svidAlloc = 0;
    }
    if (g_svidDll != 0) {
        FreeLibrary(g_svidDll);
        g_svidDll = 0;
    }
    return 1;
}
