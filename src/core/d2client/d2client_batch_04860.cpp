// src/core/d2client/d2client_batch_04860.cpp -- seven UNMAPPED 1.13c D2Client.dll rows reimplemented from
// the decompiles in build-wasm/ghidra-d2client/decomp/0x<RVA>.c (alpha lane). Base 0x6FAB0000; every
// RVA below is retail's, and every global this file owns is a static named at its retail RVA.
//
// All seven are MSVC 2003 Release CRT glue the linker pulled into the DLL:
// __lock, __SEH_prolog, __amsg_exit, __mtinitlocknum, __exit, __NMSG_WRITE and __lock_file2. Their
// observable is a
// host operation (a kernel critical section, the SEH chain head, a process exit, a message), so every
// unrecovered CRT callee is a host seam AFTER the retail arguments, which keep their order (the
// src/core/d2client/d2client_batch_1e70.cpp pattern).
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <emscripten.h>
#include <Windows.h>   // EnterCriticalSection: host (kernel) state, a no-op under wasm

// Retail module state, all zero-initialised.
static uint8_t  g_csTable[0x200];   // +0x0DB0C8  DAT_6fb8b0c8: CRT lock table, 8-byte record stride
static uint32_t g_exceptionList;    // FS:[0] SEH chain head (single-threaded here)
static uint32_t g_errorMode;        // +0x0FA8F8  DAT_6fbaa8f8: __set_error_mode flag
static uint32_t g_appType;          // +0x0FA8FC  DAT_6fbaa8fc: _app_type

struct D2ClientBatch04860Env {
    int32_t  (*mtinitlocknum)(int32_t locknum);                                     // 0x47C0
    void     (*amsg_exit)(int32_t code);                                            // 0x46DA
    void     (*ff_msgbanner)();                                                     // 0x9046
    void     (*nmsg_write)(int32_t code);                                           // 0x8ECE
    void     (*exit_ptr)(uint32_t code);                                            // [0x0DB0C0] __exit slot
    void     (*doexit)(uint32_t code, uint32_t quick, uint32_t returning_);         // 0x35A1
    void*    (*malloc_)(uint32_t size);                                             // _malloc
    void     (*free_)(void* p);                                                     // _free
    int32_t  (*init_cs)(void* cs, uint32_t spin);                                   // 0x915E ___crtInitCritSecAndSpinCount
    void     (*unlock10)();                                                         // 0x4857 _unlock(10)
    int32_t* (*errno_)(void);                                                       // 0x579C _errno
    const uint32_t* msg_table;                                                      // +0x0FB840 0x13 {code,text} pairs
    void     (*write_stderr)(const char* text, uint32_t len);                       // GetStdHandle(-12)+WriteFile
    uint32_t (*module_file_name)(char* buf, uint32_t size);                         // GetModuleFileNameA(NULL,..)
    void     (*message_box)(const char* text, const char* caption, uint32_t type);  // ___crtMessageBoxA
};

// Each lock slot is an 8-byte record: [0] = the CRITICAL_SECTION pointer, [1] = debug word. Ghidra's
// `(&DAT_6fb8b0c8)[i * 2]` dereferences the table, so the *value* stored in the slot is the pointer.
static uint32_t* lock_slot(uint32_t idx) { return (uint32_t*)(g_csTable + (size_t)idx * 8); }

// 1.13c D2Client+0x04860 (__lock): if the lock's slot is empty, initialise it with __mtinitlocknum; a
// zero return from that is fatal (__amsg_exit(0x11)). Then enter the critical section the slot holds.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_4860(int32_t locknum, const D2ClientBatch04860Env* env) {
    uint32_t* slot = lock_slot((uint32_t)locknum);
    if (slot[0] == 0 && env->mtinitlocknum(locknum) == 0)
        env->amsg_exit(0x11);
    EnterCriticalSection((LPCRITICAL_SECTION)(uintptr_t)slot[0]);
}

// 1.13c D2Client+0x055BF (FID_conflict:__lock_file2): a stream lock reached only from __fsopen ->
// __getstream. An index below 0x14 is CRT lock index+0x10; any other stream enters the critical
// section embedded in the FILE at +0x20.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_55bf(int32_t index, void* file, const D2ClientBatch04860Env* env) {
    if (index < 0x14) {
        d2_client_4860(index + 0x10, env);
        return;
    }
    EnterCriticalSection((LPCRITICAL_SECTION)((uint8_t*)file + 0x20));
}

// 1.13c D2Client+0x04904 (__SEH_prolog, injected): save EBX/ESI/EDI and the return address into the
// caller's frame at frame - size + {0xC,0x8,0x4,0x0}, then publish the frame's exception
// registration record (frame + 0x10) as the SEH chain head. Operands belong to the caller's frame, so
// they are arguments here; the return-address store is the only half retail's ABI would do itself.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_4904(uint8_t* frame, int32_t frame_size,
                                        uint32_t ebx, uint32_t esi, uint32_t edi, uint32_t retaddr) {
    uint8_t* save = frame - (size_t)frame_size;
    *(uint32_t*)(save + 0xC) = ebx;
    *(uint32_t*)(save + 0x8) = esi;
    *(uint32_t*)(save + 0x4) = edi;
    *(uint32_t*)(save + 0x0) = retaddr;
    g_exceptionList = (uint32_t)(uintptr_t)(frame + 0x10);
}

// 1.13c D2Client+0x046DA (__amsg_exit): the banner runs when the error mode selects it (mode 1, or
// mode 0 with _app_type 1); then __NMSG_WRITE(code), then __exit(0xFF) through its CRT slot. Never
// returns in retail; the exit leg is a host seam because a browser host cannot end the process.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_46da(int32_t code, const D2ClientBatch04860Env* env) {
    if (g_errorMode == 1 || (g_errorMode == 0 && g_appType == 1))
        env->ff_msgbanner();
    env->nmsg_write(code);
    env->exit_ptr(0xFF);
}

// 1.13c D2Client+0x047C0 (__mtinitlocknum): for an empty lock slot, allocate a 0x18-byte critical-
// section holder, take __lock(10), initialise the section with spin count 4000 and publish it. A
// failed allocation or initialisation latches errno 0xC (ENOMEM) and returns 0 (the SEH finally
// releases lock 10, which on the failure path runs through the unwind, hence the unlock before the
// early return). A slot that is already taken returns 1 without allocating.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_client_47c0(int32_t locknum, const D2ClientBatch04860Env* env) {
    uint32_t* slot = lock_slot((uint32_t)locknum);
    if (slot[0] != 0) return 1;
    void* mem = env->malloc_(0x18);
    if (mem == nullptr) {
        *env->errno_() = 0xC;
        return 0;
    }
    d2_client_4860(10, env);                     // __lock(10)
    if (slot[0] == 0) {
        if (env->init_cs(mem, 4000) == 0) {
            env->free_(mem);
            *env->errno_() = 0xC;
            env->unlock10();                     // __local_unwind2 runs the finally: _unlock(10)
            return 0;
        }
        slot[0] = (uint32_t)(uintptr_t)mem;
    } else {
        env->free_(mem);                         // another thread won: drop ours
    }
    env->unlock10();
    return 1;
}

// 1.13c D2Client+0x03675 (__exit): cdecl wrapper -- doexit(code, quick = 1, returning = 0).
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_3675(uint32_t code, const D2ClientBatch04860Env* env) {
    env->doexit(code, 1, 0);
}

// 1.13c D2Client+0x08ECE (__NMSG_WRITE): look the code up in the 19-entry {code,text} CRT message
// table; an absent code returns. When the error mode selects stderr, write that entry's text to
// standard error; otherwise (for any code but 0xFC) build "Runtime Error!\n\nProgram: " + module
// path + "\n\n" + text and show it in a modal box. A module path that cannot fit with its terminator
// in 60 bytes gets "..." written at len - 59. The security-cookie prologue is an injected check and
// has no engine-visible effect. Retail alloca's the text buffer, sized by both strings.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_client_8ece(int32_t code, const D2ClientBatch04860Env* env) {
    const uint32_t* table = env->msg_table;
    uint32_t i = 0;
    for (; i < 0x13 && code != (int32_t)table[i * 2]; ++i) {}
    if (i == 0x13) return;
    const char* text = (const char*)(uintptr_t)table[i * 2 + 1];
    if (g_errorMode == 1 || (g_errorMode == 0 && g_appType == 1)) {
        env->write_stderr(text, (uint32_t)std::strlen(text));
    } else if (code != 0xFC) {
        char name[0x104];
        name[0] = '\0';
        if (env->module_file_name(name, 0x104) == 0)
            std::strcpy(name, "<program name unknown>");
        size_t len = std::strlen(name);
        if (len + 1 > 60) std::strncpy(name + len - 59, "...", 3);
        char msg[0x300];                         // name[0x104] + text + the literals, retail alloca
        std::strcpy(msg, "Runtime Error!\n\nProgram: ");
        std::strcat(msg, name);
        std::strcat(msg, "\n\n");
        std::strcat(msg, text);
        env->message_box(msg, "Microsoft Visual C++ Runtime Library", 0x12010);
    }
}
