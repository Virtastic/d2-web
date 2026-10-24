// Prior art used: D2MOO Storm is an export-only map; squall Region.hpp supplies the
// SRgnCreate/Delete contracts. Retail 1.13c cached bodies and x86 call sites settle
// ESI event keys, the log allocation's EDI id, and the region record offsets.
#include <cstdint>
#include <cstddef>

// Native links and host handles are separate from retail's 32-bit ids/counts.
#include "storm_state.h"
struct StormState3b1e0 {
    uint32_t lastError, nextLogId;
    StormLog3b1e0* logs[4];
};
struct StormEventNode3b1e0 {
    StormEventNode3b1e0* next;
    StormEventNode3b1e0* previous;
    const void* messageData;
};
// List and lock values below are module-relative retail identities, never pointers.
// Internal callees are explicit ABI seams, including hidden register arguments.
// State belongs to the caller, so all twelve entries observe the same module.
struct StormCalls3b1e0 {
    void (*error)(uint32_t);
    int32_t (*strnicmp)(const char*, const char*, uint32_t);
    char* (*strupr)(char*);
    void (*enter)(uint32_t); void (*leave)(uint32_t);
    StormEventNode3b1e0* (*eventNode)(uint32_t list, uint32_t arg2, uint32_t flags, uint32_t insertionMode); // 0x1560; EDI is consumed by 0x4390
    const void* (*resolveMessageData)(uint32_t retailToken);
    void (*event)(uint32_t command, uint32_t key, uint32_t, uint32_t); // 0x34410, ESI
    uint32_t (*regionId)(uint32_t list); // 0x24f0, list 0x458f0
    uint32_t* (*regionCreate)(uint32_t list, uint32_t, uint32_t, uint32_t); // 0x25f0
    void* (*regionFind)(uint32_t list, uint32_t); // 0x2690, ESI handle
    void (*regionClear)(void*); // 0x3000, EAX node
    void (*regionRelease)(uint32_t list, void*); // list vtable slot 0
    void* (*createFile)(const char*, uint32_t access, uint32_t sharing, void* security,
                        uint32_t disposition, uint32_t attributes, void* templateFile);
    StormLog3b1e0* (*logAllocate)(uint32_t id, uint32_t* bucket, uint32_t create); // 0x347b0
    void (*write)(void*, const void*, uint32_t, uint32_t*, void* overlapped);
    void (*flush)(void*); void (*close)(void*);
    void (*logRelease)(StormLog3b1e0*, uint32_t bucket); // 0x346e0 also releases bucket lock
};
namespace {
constexpr uint32_t regionLock = 0x45940, eventLock = 0x458c8;
uint32_t logLock(uint32_t bucket) { return 0x42f60 + bucket * 0x18; }
void invalid(StormState3b1e0& s, const StormCalls3b1e0& c) { s.lastError = 0x57; c.error(0x57); }
void flushLog(StormLog3b1e0& n, const StormCalls3b1e0& c, uint32_t& written) {
    if (n.used) { c.write(n.file, n.data, n.used, &written, nullptr); c.flush(n.file); n.used = 0; n.position = 0; }
}
}
// 1.13c Storm+0x3B1E0
extern "C" int32_t Storm_Sub3B1E0(const char* a, const char* b, uint32_t count,
                                  StormState3b1e0& s, const StormCalls3b1e0& c) {
    if (!a || !b) { invalid(s,c); return static_cast<int32_t>(static_cast<uint32_t>(reinterpret_cast<uintptr_t>(a)) - static_cast<uint32_t>(reinterpret_cast<uintptr_t>(b))); }
    return c.strnicmp(a,b,count);
}
// 1.13c Storm+0x3A860
extern "C" void Storm_Sub3A860(char* text, const StormCalls3b1e0& c) { c.strupr(text); }
// 1.13c Storm+0x343D0
extern "C" uint32_t Storm_Sub343D0(const void* messageData, const StormCalls3b1e0& c) {
    c.enter(eventLock); c.eventNode(0x458bc,0,0,2)->messageData = messageData; c.leave(eventLock); return 1;
}
// #511 is SMsgBreakHandlerChain, not handler registration (D2MOO Storm.h).
// 0x35DD0 compares this identity to its message-data pointer; it never calls it.
// Retail captures carry 32-bit data tokens; resolve them at the host boundary.
extern "C" uint32_t Storm_BreakRetailMessageChain3b1e0(uint32_t token, const StormCalls3b1e0& c) {
    return Storm_Sub343D0(c.resolveMessageData(token), c);
}
// also Storm 0x34530
extern "C" void Storm_Sub34530(uint32_t key, uint32_t a, uint32_t b, const StormCalls3b1e0& c) { c.event(0x534d5348,key,a,b); }
// also Storm 0x34510
extern "C" void Storm_Sub34510(uint32_t key, uint32_t a, uint32_t b, const StormCalls3b1e0& c) { c.event(0x534d5349,key,a,b); }
// also Storm 0x344F0
extern "C" void Storm_Sub344F0(uint32_t key, uint32_t a, uint32_t b, const StormCalls3b1e0& c) { c.event(0x534d534a,key,a,b); }
// also Storm 0x344D0
extern "C" void Storm_Sub344D0(uint32_t key, uint32_t a, uint32_t b, const StormCalls3b1e0& c) { c.event(0x534d5347,key,a,b); }
// 1.13c Storm+0x30B70
extern "C" void Storm_Sub30B70(uint32_t* out, uint32_t reserved, StormState3b1e0& s, const StormCalls3b1e0& c) {
    if (out) *out = 0;
    if (!out || reserved) { invalid(s,c); return; }
    c.enter(regionLock);
    *out = c.regionId(0x458f0);
    uint32_t* n = c.regionCreate(0x458f0,*out,0,0);
    // Retail byte offsets, independent of native pointer size; all are scalar words.
    n[0x1c/4] = n[0x2c/4] = n[0x3c/4] = 0;
    n[0x48/4] = n[0x4c/4] = n[0x50/4] = n[0x54/4] = 0x7f7fffff;
    n[0x58/4] = n[0x5c/4] = 0;
    c.leave(regionLock);
}
// 1.13c Storm+0x30580
extern "C" void Storm_Sub30580(uint32_t handle, StormState3b1e0& s, const StormCalls3b1e0& c) {
    if (!handle) { invalid(s,c); return; }
    c.enter(regionLock);
    void* n = c.regionFind(0x458f0,handle);
    if (!n) c.leave(regionLock);
    c.regionClear(n); c.regionRelease(0x458f0,n);
    if (n) c.leave(regionLock);
}
// 1.13c Storm+0x34BA0
extern "C" void Storm_Sub34BA0(uint32_t id, StormState3b1e0& s, const StormCalls3b1e0& c) {
    if (!id) return;
    uint32_t bucket = id & 3;
    c.enter(logLock(bucket));
    auto* n = s.logs[bucket];
    while (n && n->id != id) n = n->next;
    if (!n) { c.leave(logLock(bucket)); return; }
    flushLog(*n,c,id); c.close(n->file); c.logRelease(n,bucket);
}
// 1.13c Storm+0x347B0: find/create the log node for id (EDI) in bucket id&3; on success returns
// with the bucket lock HELD and *bucket=id&3; on failure lock released, *bucket=~0.
extern "C" StormLog3b1e0* Storm_Sub347B0(uint32_t id, uint32_t* bucket, uint32_t create,
                                         StormState3b1e0& s, const StormCalls3b1e0& c,
                                         StormLog3b1e0* (*alloc)()) {
    if (!id) { *bucket = ~0u; return nullptr; }
    uint32_t b = id & 3;
    c.enter(logLock(b));
    *bucket = b;
    StormLog3b1e0** link = &s.logs[b];
    for (auto* n = *link; n; n = *link) {
        if (n->id == id) return n;
        link = &n->next;
    }
    if (create) {
        auto* n = alloc(); // VirtualAlloc(0x1001c)
        *link = n; // retail stores the result even when null
        if (n) { n->id = id; n->next = nullptr; n->file = reinterpret_cast<void*>(~uintptr_t{0});
                 n->used = 0; n->position = 0; return n; }
    }
    c.leave(logLock(b));
    *bucket = ~0u;
    return nullptr;
}
// 1.13c Storm+0x34AE0
extern "C" uint32_t Storm_Sub34AE0(const char* name, uint32_t /*unused*/, uint32_t* out,
                                    StormState3b1e0& s, const StormCalls3b1e0& c) {
    // Retail dereferences name before validating it; preserve that precondition.
    char first = *name;
    if (out) *out = 0;
    if (!out || !name || !first) { invalid(s,c); return 0; }
    void* file = c.createFile(name,0x40000000,3,nullptr,2,0x80,nullptr);
    if (file == reinterpret_cast<void*>(~uintptr_t{0})) return 0;
    uint32_t id = ++s.nextLogId;
    *out = id;
    uint32_t bucket;
    auto* n = c.logAllocate(id,&bucket,1);
    if (!n) { *out = 0; return 0; } // retail leaves file open on allocation failure
    n->file = file; n->field18 = 1; n->field14 = 0;
    c.leave(logLock(bucket)); return 1;
}
// 1.13c Storm+0x349E0
extern "C" void Storm_Sub349E0(uint32_t scratch, StormState3b1e0& s, const StormCalls3b1e0&) {
    for (int i = 0; i < 4; ++i) g_stormLogs[i] = s.logs[i]; // caller-owned state mirrors the module's
    Storm_FlushLogs_349E0(scratch);
}
// also Storm 0x34410: ensure a group node keyed ESI exists in the list at 0x458e0, then dispatch
// 0x35CD0(command, key, a, b). The list walk and 0x15E0 creation are seams (list identity 0x458e0).
struct StormEvent34410Calls {
    bool (*hasGroup)(uint32_t list, uint32_t key);
    void (*createGroup)(uint32_t list, uint32_t key); // 0x15E0, node[2] = key
    void (*dispatch)(uint32_t command, uint32_t key, uint32_t a, uint32_t b); // 0x35CD0
};
extern "C" void Storm_Sub34410(uint32_t command, uint32_t a, uint32_t b, uint32_t key,
                               const StormEvent34410Calls& c) {
    if (!c.hasGroup(0x458e0, key)) c.createGroup(0x458e0, key);
    c.dispatch(command, key, a, b);
}
