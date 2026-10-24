// src/core/fog/fog_batch_n04.cpp -- ten 1.13c Fog.dll rows from the cached Ghidra decompiles
// (build-wasm/ghidra-fog/decomp/0x<RVA>.c), alpha lane: no spec, no vector. Win32/CRT/sibling-row
// callees go through FogN04Seams so a host (wasm: no-ops/stdio) supplies them.
#include <cstddef>
#include <cstdint>
#include <cstdio>

struct FogN04Seams {
    void (*validateCs)(void* cs);                       // Fog+0x11690
    void (*enterCs)(void* cs);                          // EnterCriticalSection
    void (*leaveCs)(void* cs);                          // LeaveCriticalSection
    void (*deleteCs)(void* cs);                         // DeleteCriticalSection
    uint32_t (*nameOf)(uint32_t handle);                // Fog import Ordinal_11 (handle -> string)
    void (*strCopy)(uint32_t a, uint32_t b, void* c);   // Fog import Ordinal_501, argument order as retail pushes it
    void (*free_)(void* p);                             // Fog+0x1B0C0
    void (*sub5E550)(void);                             // Fog+0xE550
    void (*sub64260)(void);                             // Fog+0x14260
    void (*closeSocket)(uint32_t s);                    // WSOCK32 Ordinal_3 closesocket
    uint32_t (*getThreadContext)(void* hThread, uint32_t* ctx);  // GetThreadContext, 0xB3-dword CONTEXT
    void (*logPrint)(void* ctx, const char* text);      // Fog+0x18AF0 target (Fog+0x18770)
    void (*dumpBytes)(void* log, void* ctx, const void* p, uint32_t n, uint32_t a, uint32_t b);  // Fog+0x18950
    void (*fatal)(int32_t line);                        // Fog+0xFC20/Ordinal_116/FogError(6,..)/Fog+0x184A0/_exit(-1)
    uint32_t (*tickCount)(void);                        // GetTickCount
    void (*sleepMs)(uint32_t ms);                       // Sleep
    uint32_t (*cpuidLeaf1)(uint32_t out[4]);            // cpuid(1): eax,ebx,ecx,edx
    void (*fclose_)(void* f);                           // _fclose
};
FogN04Seams g_fogN04;

// Fog+0x2D8F8 .. : log FILE table (5 slots), the log CS, and the pending-writer counter.
void* g_fogN04LogFiles[5];
uint8_t g_fogN04LogCs[24];
uint8_t g_fogN04PendingCs[24];
int32_t g_fogN04Pending;
int32_t g_fogN04LogReady;

static inline uint32_t rd(const void* p, uint32_t o) { return *reinterpret_cast<const uint32_t*>(static_cast<const uint8_t*>(p) + o); }

// 1.13c Fog+0xD940
extern "C" void fog_sub_D940(void* table, uint32_t key, uint8_t* out) {
    const uint32_t bucket = rd(table, 0xC) & key;
    uint8_t* cs = reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(rd(table, 0x1C))) + bucket * 0x18;
    g_fogN04.validateCs(cs);
    g_fogN04.enterCs(cs);
    const uint32_t* e = reinterpret_cast<const uint32_t*>(static_cast<uintptr_t>(
        reinterpret_cast<const uint32_t*>(static_cast<uintptr_t>(rd(table, 0x14)))[bucket]));
    while (e != nullptr && e[0] != key) e = reinterpret_cast<const uint32_t*>(static_cast<uintptr_t>(e[1]));
    if (e == nullptr) {
        g_fogN04.leaveCs(cs);
        *out = 0;
        return;
    }
    g_fogN04.strCopy(key, g_fogN04.nameOf(e[6]), out);
    g_fogN04.leaveCs(cs);
}

// 1.13c Fog+0xEAF0
extern "C" void fog_sub_EAF0(void) { g_fogN04.sub5E550(); }

// 1.13c Fog+0x14530
extern "C" void fog_sub_14530(void) { g_fogN04.sub64260(); }

// 1.13c Fog+0x14540 -- appends the crash-dump record to the log file. The retail body reads its
// record out of the caller's frame (unaff_EDI, stack0xfffffeb4), which has no wasm analogue; the
// caller passes the record explicitly. Returns 1 when the file opened and the record was written.
extern "C" int32_t fog_sub_14540(void* (*openAppend)(const char* name, const char* mode), const char* name,
                                 const void* record, uint32_t dwords) {
    void* f = openAppend(name, "ab");
    if (f == nullptr) return 0;
    std::fwrite(record, dwords * 4 + 0x10, 1, static_cast<FILE*>(f));
    g_fogN04.fclose_(f);
    return 1;
}

// 1.13c Fog+0x14C70 -- log the faulting thread's stack and code bytes.
extern "C" void fog_sub_14C70(void* hThread) {
    uint32_t ctx[0xB3] = {};
    ctx[0] = 0x10007;  // CONTEXT_FULL
    g_fogN04.getThreadContext(hThread, ctx);
    g_fogN04.logPrint(nullptr, "Stack bytes:\n");
    g_fogN04.dumpBytes(nullptr, nullptr, reinterpret_cast<const void*>(static_cast<uintptr_t>(ctx[0x1C])), 0x1000, 0, 1);  // Esp
    g_fogN04.logPrint(nullptr, "Code bytes:\n");
    g_fogN04.dumpBytes(nullptr, nullptr, reinterpret_cast<const void*>(static_cast<uintptr_t>(ctx[0x1A])), 0x20, 0, 1);    // Eip
}

// 1.13c Fog+0x16920 (ordinal 10000, SOCKET_Close): closesocket(s), result ignored.
extern "C" void fog_sub_16920(uint32_t s) { g_fogN04.closeSocket(s); }

// 1.13c Fog+0x18AF0 (ordinal 10238, LogPrintf): NULL format is fatal (line 0x160); NULL log
// context selects the default log, then the formatted write is Fog+0x18770.
extern "C" void fog_sub_18AF0(void* log, const char* text) {
    if (text == nullptr) {
        g_fogN04.fatal(0x160);
        return;
    }
    g_fogN04.logPrint(log, text);
}

// 1.13c Fog+0xE3B0 -- tail of a queue teardown: with the object's CS (+0x2C) held, free its
// +0x74 chain (next at +4), clear the head, delete the CS and free the object itself.
extern "C" void fog_sub_E3B0(uint8_t* obj) {
    void* cs = obj + 0x2C;
    g_fogN04.validateCs(cs);
    g_fogN04.enterCs(cs);
    uint32_t node = rd(obj, 0x74);
    while (node != 0) {
        const uint32_t next = rd(reinterpret_cast<void*>(static_cast<uintptr_t>(node)), 4);
        g_fogN04.free_(reinterpret_cast<void*>(static_cast<uintptr_t>(node)));
        node = next;
    }
    *reinterpret_cast<uint32_t*>(obj + 0x74) = 0;
    g_fogN04.deleteCs(cs);
    g_fogN04.free_(obj);
}

// 1.13c Fog+0x1C020 -- CPUID.1: EAX (signature) in the low dword, ECX in the high.
extern "C" uint64_t fog_sub_1C020(void) {
    uint32_t r[4] = {};
    g_fogN04.cpuidLeaf1(r);
    return (static_cast<uint64_t>(r[2]) << 32) | r[0];
}

// 1.13c Fog+0x184A0 -- log shutdown: unopened log layer is fatal (line 0x1B5); otherwise spin up
// to 1000 ms for the pending-writer counter to drain, then close the five log FILEs.
extern "C" void fog_sub_184A0(void) {
    if (!g_fogN04LogReady) {
        g_fogN04.fatal(0x1B5);
        return;
    }
    const uint32_t start = g_fogN04.tickCount();
    uint32_t elapsed = g_fogN04.tickCount() - start;
    while (elapsed < 1000) {
        g_fogN04.enterCs(g_fogN04PendingCs);
        const int32_t pending = g_fogN04Pending > 0 ? g_fogN04Pending : 0;
        g_fogN04.leaveCs(g_fogN04PendingCs);
        if (pending == 0) break;
        g_fogN04.sleepMs(10);
        elapsed = g_fogN04.tickCount() - start;
    }
    for (void*& f : g_fogN04LogFiles) {
        g_fogN04.enterCs(g_fogN04LogCs);
        if (f != nullptr) { g_fogN04.fclose_(f); f = nullptr; }
        g_fogN04.leaveCs(g_fogN04LogCs);
    }
}
