// src/core/fog/fog_batch_b144.cpp -- 1.13c Fog.dll implementation batch b144 (log file manager, queue push,
// stack-walk thread), from build-wasm/ghidra-fog/decomp/0x<RVA>.c. Base 0x6FF50000; RVAs are retail's.
// Module-internal callees and Win32 calls that the shim lacks are seams on FogB144Api. Retail
// 32-bit addresses stay uint32_t. Register-passed arguments (ESI/EDI/EBX/AX in the decompile) are
// recovered as explicit parameters; each is noted where it is a guess.
#include <cstdint>
#include <cstdio>
#include <cstring>

#include <Windows.h>

typedef struct FogB144Api {
    uint32_t (*capture_ip)(void);                                   // Fog+0xFC20
    void (*ordinal116)(void);                                       // WSOCK32 #116
    void (*fatal)(int level, const char* file, int line, const char* fmt, ...); // Fog+0x10F30
    void (*abort)(void);                                            // Fog+0x184A0
    void (*exit_process)(int code);                                 // _exit
    int (*is_bad_read_ptr)(const void* p, uint32_t n);              // IsBadReadPtr
} FogB144Api;

// Win32 calls the compat Windows.h does not carry. The host fills this in once; FILETIME is two u32
// (low, high) and file names are returned through a 260-byte buffer.
typedef struct FogWin32 {
    uint32_t (*wait)(void* h, uint32_t ms);                     // WaitForSingleObject
    uint32_t (*suspend)(void* h);                               // SuspendThread
    uint32_t (*resume)(void* h);                                // ResumeThread
    int (*thread_context)(void* h);                             // GetThreadContext, flags 0x10007
    int (*set_event)(void* h);                                  // SetEvent
    uint32_t (*last_error)(void);                               // GetLastError
    int (*systime_to_filetime)(const SYSTEMTIME* st, uint32_t ft[2]); // SystemTimeToFileTime
    int (*delete_file)(const char* path);                       // DeleteFileA
    int (*computer_name)(char* out, uint32_t* n);               // GetComputerNameA
    void* (*find_first)(const char* pat, char* name_out);       // FindFirstFileA, null on failure
    int (*find_next)(void* h, char* name_out);                  // FindNextFileA
    void (*find_close)(void* h);                                // FindClose
} FogWin32;
static const FogWin32* g_w32;

static void fogb144_die(const FogB144Api* a, int line) {
    const uint32_t ip = a->capture_ip();
    a->ordinal116();
    a->fatal(6, "", line, "Unrecoverable internal error %08x", ip);
    a->abort();
    a->exit_process(-1);
}

// Log manager state, retail addresses in the comments.
static uint8_t  g_names[20][16];     // 0x6FF816F8
static SYSTEMTIME g_startup;         // 0x6FF81838
static uint32_t g_banner[20];        // 0x6FF81850, startup banner pending
static uint32_t g_last_use[20];      // 0x6FF818A8
static HANDLE   g_thread;            // 0x6FF818A0
static FILE*    g_files[20];         // 0x6FF81900
static uint32_t g_opened[20];        // 0x6FF81950
static uint32_t g_stop;              // 0x6FF819A0
static uint32_t g_816f0, g_81848, g_818f8, g_819a4;
static uint32_t g_list_head;         // 0x6FFA6EC4 (retail address of first node)
static uint32_t g_cleanup_tick;      // 0x6FFA6ED8
static CRITICAL_SECTION g_cs_a6e8c;  // 0x6FFA6E8C
static CRITICAL_SECTION g_cs_a6ea4;  // 0x6FFA6EA4
static const char* g_log_dir = "";   // 0x6FF7F368

// --- Fog+0x15D50 ------------------------------------------------------------------------------
typedef struct FogStackWalkApi {
    FogB144Api base;
    void (*log)(const char* fmt, ...);        // Fog+0x190A0 against &DAT_6ff768ec
    void (*walk)(void);                       // Fog+0x155E0, run on the suspended victim
} FogStackWalkApi;
static HANDLE g_sw_wake, g_sw_done, g_sw_self, g_sw_victim;  // 0x6FFA6E5C, E60, E58, E64

// 1.13c Fog+0x15d50
// Stack-walk thread: wait for the wake event; if it is not signalled close the three handles and
// return 0. While the victim is not this very thread: suspend it, fetch its context
// (flags 0x10007), walk, resume, close it, signal done, wait again. Victim == self is fatal 0x11b.
extern "C"
uint32_t fog_sub_15d50(const FogStackWalkApi* api) {
    DWORD w = g_w32->wait(g_sw_wake, 0xffffffff);
    for (;;) {
        if (w != 0) {
            if (g_sw_wake) { CloseHandle(g_sw_wake); g_sw_wake = nullptr; }
            if (g_sw_done) { CloseHandle(g_sw_done); g_sw_done = nullptr; }
            if (g_sw_self) { CloseHandle(g_sw_self); g_sw_self = nullptr; }
            return 0;
        }
        if (g_sw_victim == g_sw_self) break;
        if (g_w32->suspend(g_sw_victim) == 0xffffffff) {
            api->log("[StackWalkThread]: g_w32->suspend() err = %lu", g_w32->last_error());
        } else {
            api->log("Suspended Victim Thread...");
            if (g_w32->thread_context(g_sw_victim) == 0)
                api->log("[StackWalkThread]: GetThreadContext() err = %lu", g_w32->last_error());
            else
                api->walk();
            g_w32->resume(g_sw_victim);
        }
        CloseHandle(g_sw_victim);
        g_sw_victim = nullptr;
        g_w32->set_event(g_sw_done);
        w = g_w32->wait(g_sw_wake, 0xffffffff);
    }
    fogb144_die(&api->base, 0x11b);
    return 0;
}

// --- Fog+0x17A50 ------------------------------------------------------------------------------
typedef struct FogQueueApi {
    FogB144Api base;
    void (*cs_prepare)(void* cs);             // Fog+0x11690 on the queue's critical section
    uint8_t* (*alloc)(void* ctx);             // Fog+0x1B560(0)
} FogQueueApi;

// 1.13c Fog+0x17a50
// D2QSQueue push: `size` arrives in AX, `queue` is the stack arg, `data` the second. Zero size is
// fatal 0x4c. Under the queue lock (queue+4) allocate a node (null -> fatal log, source
// D2QSQueue.cpp:0x50): [size u16][pad][next u32 at +4][u16 +2, +6 = 0]... payload from +8.
// Append at the tail: head at queue+0x1c, tail at queue+0x20; link = node->next at node+4.
extern "C"
void fog_sub_17a50(const FogQueueApi* api, uint8_t* queue, const void* data, uint16_t size) {
    if (size == 0) { fogb144_die(&api->base, 0x4c); return; }
    CRITICAL_SECTION* cs = (CRITICAL_SECTION*)(queue + 4);
    api->cs_prepare(cs);
    EnterCriticalSection(cs);
    uint8_t* node = api->alloc(nullptr);
    if (node == nullptr)
        api->base.fatal(2, "..\\Source\\Fog\\Src\\D2QSQueue.cpp", 0x50, "");
    uint16_t z = 0;
    memcpy(node, &size, 2);
    memcpy(node + 4, &z, 2);   // decompile: puVar1[2] = 0, puVar1[3] = 0 (the next link, 4 bytes)
    memcpy(node + 6, &z, 2);
    memcpy(node + 8, data, size);
    uint32_t tail;
    memcpy(&tail, queue + 0x20, 4);
    const uint32_t me = (uint32_t)(uintptr_t)node;
    uint32_t head;
    memcpy(&head, queue + 0x1c, 4);
    if (head == 0) memcpy(queue + 0x1c, &me, 4);
    else memcpy((uint8_t*)(uintptr_t)tail + 4, &me, 4);
    memcpy(queue + 0x20, &me, 4);
    LeaveCriticalSection(cs);
}

// --- Fog+0x18150 ------------------------------------------------------------------------------
// 1.13c Fog+0x18150
// Delete an old log: the file name ends "yymmdd.txt" (last 10 chars: yy mm dd). Parse them, build a
// midnight SYSTEMTIME (yy <= 0x5a -> 2000+yy else 1900+yy), convert to FILETIME and delete the file
// when `now` (high dword) is >= it and more than 0x4b6 high-dword ticks (about 14 days) later.
extern "C"
void fog_sub_18150(const char* path, const uint32_t* now) {
    int n = (int)strlen(path);
    if (n - 10 <= 0) return;
    int yy = 0, mm = 0, dd = 0;
    sscanf(path + n - 10, "%2d%2d%2d", &yy, &mm, &dd);
    SYSTEMTIME st;
    memset(&st, 0, sizeof st);
    st.wYear = (WORD)(yy < 0x5b ? yy + 2000 : yy + 1900);
    st.wMonth = (WORD)mm;
    st.wDay = (WORD)dd;
    uint32_t ft[2];
    g_w32->systime_to_filetime(&st, ft);
    if (ft[1] <= now[1] && now[1] - ft[1] > 0x4b6)
        g_w32->delete_file(path);
}

// --- Fog+0x18200 ------------------------------------------------------------------------------
// 1.13c Fog+0x18200
// Close idle log files: any open slot whose open time is >15000 ms or last-use >5000 ms old.
extern "C"
void fog_sub_18200(void) {
    const DWORD now = GetTickCount();
    for (int i = 0; i < 20; i++) {
        if (g_files[i] && (now - g_opened[i] > 15000 || now - g_last_use[i] > 5000)) {
            fclose(g_files[i]);
            g_files[i] = nullptr;
        }
    }
}

// --- Fog+0x182B0 / 0x18340 --------------------------------------------------------------------
static void fogb144_computer(char* out64) {
    DWORD n = 0x40;
    if (g_w32->computer_name(out64, &n) == 0) out64[0] = '\0';
}
static const char kRule[] = "---------------------------------------------------------------\n";

// 1.13c Fog+0x182b0   (file arrives in ESI)
// Realm-startup banner: rule, "<computer> Realm startup at Y-M-D h:m:s.ms" from the startup time
// at 0x6FF81838 (year, month, day, hour, minute, second, ms), rule.
extern "C"
void fog_sub_182b0(FILE* f) {
    char name[64];
    fogb144_computer(name);
    fputs(kRule, f);
    fprintf(f, "%s Realm startup at %04d-%02d-%02d %02d:%02d:%02d.%03u\n", name, g_startup.wYear,
            g_startup.wMonth, g_startup.wDay, g_startup.wHour, g_startup.wMinute,
            g_startup.wSecond, g_startup.wMilliseconds);
    fputs(kRule, f);
}

// 1.13c Fog+0x18340   (SYSTEMTIME in ESI, file in EDI)
// Day banner: rule, "<computer> YYYY-MM-DD" (words 0, 1 and 3 of the SYSTEMTIME), rule.
extern "C"
void fog_sub_18340(FILE* f, const uint16_t* st) {
    char name[64];
    fogb144_computer(name);
    fputs(kRule, f);
    fprintf(f, "%s %04d-%02d-%02d\n", name, st[0], st[1], st[3]);
    fputs(kRule, f);
}

// --- Fog+0x183A0 ------------------------------------------------------------------------------
// 1.13c Fog+0x183a0   (st in ESI, mode in EBX, fresh in EDI)
// Open "<logdir><name>yymmdd.txt" at the local date. If `fresh` is given it is set to 1, and for
// mode "a" cleared to 0 when the file already exists.
extern "C"
FILE* fog_sub_183a0(const char* name, SYSTEMTIME* st, const char* mode, uint32_t* fresh) {
    char path[260];
    GetLocalTime(st);
    snprintf(path, sizeof path, "%s%s%02d%02d%02d.txt", g_log_dir, name, st->wYear % 100, st->wMonth,
             st->wDay);
    if (fresh && (*fresh = 1, *mode == 'a')) {
        FILE* f = fopen(path, "r");
        if (f) { fclose(f); *fresh = 0; }
    }
    return fopen(path, mode);
}

// --- Fog+0x18630 ------------------------------------------------------------------------------
typedef struct FogCleanApi {
    FogB144Api base;
    int (*have_log_dir)(void);                       // Fog+0x143D0
    const char* log_dir;                             // DAT_6ff76e9c
    void (*finish_dir)(char* dir);                   // Fog+0x17C40
} FogCleanApi;

// 1.13c Fog+0x18630
// Once per 24 h (86400000 ms; the first call always): unlock cs a6ea4, run Fog+0x184A0, relock,
// take the local time as FILETIME, build the log dir (empty unless Fog+0x143D0), and call
// Fog+0x18150 on every "<dir>d2*.txt". The per-file path format DAT_6ff76fb4 is assumed "%s%s".
extern "C"
void fog_sub_18630(const FogCleanApi* api) {
    if (g_cleanup_tick != 0 && GetTickCount() - g_cleanup_tick <= 86399999) return;
    g_cleanup_tick = GetTickCount();
    LeaveCriticalSection(&g_cs_a6ea4);
    api->base.abort();
    EnterCriticalSection(&g_cs_a6ea4);
    SYSTEMTIME st;
    GetLocalTime(&st);
    uint32_t now[2];
    g_w32->systime_to_filetime(&st, now);
    char dir[260];
    dir[0] = '\0';
    if (api->have_log_dir()) {
        snprintf(dir, sizeof dir, "%s", api->log_dir);
        api->finish_dir(dir);
    }
    char pat[260], full[260];
    snprintf(pat, sizeof pat, "%sd2*.txt", dir);
    char fname[260];
    void* h = g_w32->find_first(pat, fname);
    if (!h) return;
    do {
        snprintf(full, sizeof full, "%s%s", dir, fname);
        fog_sub_18150(full, now);
    } while (g_w32->find_next(h, fname));
    g_w32->find_close(h);
}

// --- Fog+0x18BB0 ------------------------------------------------------------------------------
typedef struct FogMgrApi {
    void (*unlink)(uint32_t node);                   // Fog+0x1040
    void (*free)(uint32_t node, const char* file, int line, int flag); // Storm #403
} FogMgrApi;

// 1.13c Fog+0x18bb0
// Log manager shutdown: raise the stop flag, wait up to 30 s on the worker thread and close it,
// then under cs a6e8c unlink and free every node on the global list ("<AUMessage LogManager>"),
// and zero the manager state.
extern "C"
void fog_sub_18bb0(const FogMgrApi* api) {
    g_stop = 1;
    g_w32->wait(g_thread, 30000);
    CloseHandle(g_thread);
    EnterCriticalSection(&g_cs_a6e8c);
    while ((int32_t)g_list_head > 0) {
        const uint32_t n = g_list_head;
        api->unlink(n);
        api->free(n, "<AUMessage LogManager>", -2, 0);
    }
    g_816f0 = 0;
    g_81848 = 0;
    g_thread = nullptr;
    g_819a4 = 0;
    g_818f8 = 0;
    g_stop = 0;
    LeaveCriticalSection(&g_cs_a6e8c);
}

// --- Fog+0x18D50 ------------------------------------------------------------------------------
// 1.13c Fog+0x18d50
// Find or open the log slot for `name` (<16 chars), return its FILE* and the slot in *slot_out.
// Scan from slot (name[0] != 0) so slot 0 is the nameless one. A matching open slot is returned; a
// matching closed one is reopened. Otherwise reuse the first empty-named slot, else the closed slot
// chosen by the last-use rule, else the open slot with the oldest last use (closed first).
// The return value is not visible in the decompile; it is taken as the slot's FILE*.
extern "C"
FILE* fog_sub_18d50(const FogB144Api* api, const uint8_t* name, uint32_t* slot_out) {
    const DWORD now = GetTickCount();
    int empty = -1, cand = -1, oldest = 1;
    int idx = -1;
    for (int i = (name[0] != 0); i < 20; i++) {
        if (strcmp((const char*)g_names[i], (const char*)name) == 0) {
            if (g_files[i]) { if (slot_out) *slot_out = (uint32_t)i; return g_files[i]; }
            idx = i;
            break;
        }
        if (g_names[i][0] == 0) {
            if (empty < 0) empty = i;
        } else if (g_files[i] == nullptr) {
            if (cand < 0 || g_last_use[cand] < now - g_last_use[i]) cand = i;
        } else if (now - g_last_use[oldest] < now - g_last_use[i]) {
            oldest = i;
        }
    }
    if (idx < 0) {
        idx = empty >= 0 ? empty : cand >= 0 ? cand : oldest;
        if (empty < 0 && g_files[idx]) { fclose(g_files[idx]); g_files[idx] = nullptr; }
    }
    if (strlen((const char*)name) > 15) fogb144_die(api, 0x335);
    strcpy((char*)g_names[idx], (const char*)name);
    SYSTEMTIME st;
    uint32_t fresh;   // set by the open; the retail caller's use of it is not recoverable
    FILE* f = fog_sub_183a0((const char*)name, &st, "a", &fresh);
    g_opened[idx] = now;
    g_last_use[idx] = now;
    const uint32_t pending = g_banner[idx];
    g_files[idx] = f;
    if (pending == 0 || f == nullptr) {
        fog_sub_18340(f, (const uint16_t*)&st);
    } else {
        fog_sub_182b0(f);
        g_banner[idx] = 0;
    }
    if (g_files[idx] && slot_out) *slot_out = (uint32_t)idx;
    return g_files[idx];
}

// --- Fog+0x18F90 ------------------------------------------------------------------------------
// 1.13c Fog+0x18f90   (record in EDI)
// Write a type-1 log record: +0x0c type (else fatal 0x28a), +0x10 timestamp flag, +0x1c..0x22
// h/m/s/ms, +0x24 "name\0text\0". Optional "hh:mm:ss.mmm  " prefix, then the text; stamps the
// slot's last-use tick.
extern "C"
void fog_sub_18f90(const FogB144Api* api, const uint8_t* rec) {
    uint32_t type, stamp;
    memcpy(&type, rec + 0xc, 4);
    if (type != 1) { fogb144_die(api, 0x28a); return; }
    const char* name = (const char*)(rec + 0x24);
    const char* text = name + strlen(name) + 1;
    uint32_t slot = 0;
    FILE* f = fog_sub_18d50(api, (const uint8_t*)name, &slot);
    if (!f) return;
    memcpy(&stamp, rec + 0x10, 4);
    if (stamp) {
        uint16_t t[4];
        memcpy(t, rec + 0x1c, 8);
        fprintf(f, "%02d:%02d:%02d.%03d  ", t[0], t[1], t[2], t[3]);
    }
    fwrite(text, strlen(text), 1, f);
    g_last_use[slot] = GetTickCount();
}

// --- Fog+0x19160 ------------------------------------------------------------------------------
static void fogb144_cat(char* dst, const char* s, uint32_t cap) {
    uint32_t n = (uint32_t)strlen(dst);
    while (*s && n + 1 < cap) dst[n++] = *s++;
    dst[n] = '\0';
}

// 1.13c Fog+0x19160   (record in EAX)
// Write a type-2 hex-dump record (else fatal 0x36b). After "name\0" comes {+0 data addr, +4 len,
// +8 short-address flag, +0xc inline flag, +0x10 ascii flag, +0x14 label\0 [payload]}. 16 bytes per
// line: "<stamp or 14 spaces><label><addr>:" then " xx" groups of four, then optionally padding and
// the ASCII column ((b & 0x7f) < 0x20 -> '.'), or "<unreadable>" if IsBadReadPtr. Address is the
// stored base + offset (%08X), or just the offset (%04X) when the short flag is set.
// `data_ptr` stands for the retail 32-bit data pointer when the payload is not inline.
extern "C"
void fog_sub_19160(const FogB144Api* api, const uint8_t* rec, const uint8_t* data_ptr) {
    uint32_t type;
    memcpy(&type, rec + 0xc, 4);
    if (type != 2) { fogb144_die(api, 0x36b); return; }
    const char* name = (const char*)(rec + 0x24);
    const uint8_t* p = (const uint8_t*)name + strlen(name) + 1;
    uint32_t base, len, shortaddr, inl, ascii;
    memcpy(&base, p, 4);
    memcpy(&len, p + 4, 4);
    memcpy(&shortaddr, p + 8, 4);
    memcpy(&inl, p + 0xc, 4);
    memcpy(&ascii, p + 0x10, 4);
    const char* label = (const char*)(p + 0x14);
    const uint8_t* data = inl == 0 ? (const uint8_t*)label + strlen(label) + 1 : data_ptr;
    uint32_t slot;
    FILE* f = fog_sub_18d50(api, (const uint8_t*)name, &slot);
    if (!f) return;
    uint16_t t[4];
    memcpy(t, rec + 0x1c, 8);
    char stamp[0x20], pad[0x20];
    snprintf(stamp, sizeof stamp, "%02d:%02d:%02d.%03d  ", t[0], t[1], t[2], t[3]);
    snprintf(pad, sizeof pad, "%s", "              ");
    const uint8_t* start = data;
    bool first = true;
    while (len != 0) {
        const uint32_t off = (uint32_t)(data - start);
        const uint32_t a = inl == 0 ? base + off : off;
        const uint32_t n = len > 15 ? 0x10 : len;
        fprintf(f, shortaddr == 0 ? "%s%s%08X:" : "%s%s%04X:", first ? stamp : pad, label, a);
        first = false;
        char buf[0x110];
        buf[0] = '\0';
        if (api->is_bad_read_ptr(data, n) == 0) {
            for (uint32_t i = 0; i < n; i++) {
                char b[8];
                snprintf(b, sizeof b, (i & 3) == 0 ? " %02X" : "%02X", data[i]);
                fogb144_cat(buf, b, 0x100);
            }
            if (ascii) {
                for (uint32_t k = ((0x10 - n) * 9 >> 2) + 2; k; k--) fogb144_cat(buf, " ", 0x100);
                size_t at = strlen(buf);
                for (uint32_t i = 0; i < n; i++) {
                    uint8_t c = data[i];
                    buf[at + i] = (c & 0x7f) < 0x20 ? '.' : (char)c;
                }
                buf[at + n] = '\0';
            }
        } else {
            snprintf(buf, sizeof buf, "<unreadable>");
        }
        fprintf(f, "%s\n", buf);
        data += n;
        len -= n;
    }
}
