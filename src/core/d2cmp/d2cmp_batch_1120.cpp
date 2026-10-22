// src/core/d2cmp/d2cmp_batch_1120.cpp -- twelve UNMAPPED 1.13c D2CMP.dll rows (the UCELLIST
// hash-table / explicit-list destructor family) from the cached Ghidra decompiles. Alpha batch
// lane: implementation + marker only, no spec, no vector. Guest pointers are 32-bit addresses, read
// through uint32_t* as src/core/d2client/d2client_2a80.cpp does. SEH frame setup is dropped.
// Callees with no port yet (0x1840 0x1880 0x1920 0x16C0 and the Storm #403 free) are env seams.
// Register-passed `this` of 0x1840 (EDI) and 0x1880 (EBX) is inferred as the caller's object
// (param_1 in 0x1300 / 0x13A0); no disassembly was available to confirm it.
#include <cstdint>
#include <Windows.h>

struct D2CmpListEnv {
    void (*f1840)(uint32_t* self);            // 0x1840 free chunk/cell lists of self
    void (*f1880)(uint32_t* self, int arg);   // 0x1880
    void (*f1920)(uint32_t* list);            // 0x1920
    void (*f16c0)(uint32_t* list);            // 0x16C0
    void (*smemFree)(void* p, const char* file, int32_t line, uint32_t flags); // Storm #403
};

static const char kTsList[] = "__AV__TSExplicitList_UCELLIST__";   // retail tag at 0x6FE34144 (name as in the d2client ports)

// Unlink the node-owner link pair {first, prev-or-~head} at `lk` (shared by 0x15D0 / 0x1680).
static void unlink_link(uint32_t* lk) {
    uint32_t first = lk[0];
    if (!first) return;
    uint32_t prev = lk[1];
    uint32_t* slot = ((int32_t)prev < 0) ? (uint32_t*)(uintptr_t)~prev
                                         : (uint32_t*)((uintptr_t)lk + (prev - ((uint32_t*)(uintptr_t)first)[1]));
    *slot = first;
    ((uint32_t*)(uintptr_t)lk[0])[1] = lk[1];
    lk[0] = 0;
    lk[1] = 0;
}

extern "C" {

// 1.13c D2CMP+0x01680 (EAX = link pair)
void d2cmp_1680(uint32_t* link) { unlink_link(link); }

// 1.13c D2CMP+0x015D0: unlink the two link pairs at +0xC and +4.
void d2cmp_15d0(uint32_t* self) {
    unlink_link(self + 3);
    unlink_link(self + 1);
}

// 1.13c D2CMP+0x01580: 0x16C0 on +0x2C, then 0x15D0.
void d2cmp_1580(uint32_t* self, const D2CmpListEnv* e) {
    e->f16c0(self + 0xb);
    d2cmp_15d0(self);
}

// 1.13c D2CMP+0x01490
void d2cmp_1490(uint32_t* list, const D2CmpListEnv* e) { e->f1920(list); }

// 1.13c D2CMP+0x01500: run 0x1920 on each 12-byte cell, then free the array if present.
void d2cmp_1500(uint32_t* self, const D2CmpListEnv* e) {
    for (uint32_t i = 0; i < self[1]; i++) e->f1920((uint32_t*)(uintptr_t)(self[2] + i * 0xc));
    if (self[2]) e->smemFree((void*)(uintptr_t)self[2], kTsList, (int32_t)0xfffffffe, 0);
}

// 1.13c D2CMP+0x013A0: base-class destructor body (vtable PTR_6fe2fe38).
void d2cmp_13a0(uint32_t* self, const D2CmpListEnv* e) {
    self[0] = 0x6fe2fe38;
    e->f1880(self, 1);
    d2cmp_1500(self + 5, e);
    e->f1920(self + 1);
}

// 1.13c D2CMP+0x01300: derived destructor body (vtable PTR_6fe2fe08).
void d2cmp_1300(uint32_t* self, const D2CmpListEnv* e) {
    self[0] = 0x6fe2fe08;
    e->f1840(self);
    e->f1920(self + 0xe);
    e->f1920(self + 10);
    d2cmp_13a0(self, e);
}

// 1.13c D2CMP+0x01120: DeleteCriticalSection(+0x50), then 0x1300.
void d2cmp_1120(uint32_t* self, const D2CmpListEnv* e) {
    DeleteCriticalSection((LPCRITICAL_SECTION)(self + 0x14));
    d2cmp_1300(self, e);
}

// 1.13c D2CMP+0x01170
void d2cmp_1170(uint32_t* self, const D2CmpListEnv* e) { d2cmp_1300(self, e); }

// 1.13c D2CMP+0x01430: scalar deleting destructor over 0x1120.
uint32_t* d2cmp_1430(uint32_t* self, uint8_t flags, const D2CmpListEnv* e) {
    d2cmp_1120(self, e);
    if ((flags & 1) && self) e->smemFree(self, "delete", -1, 0);
    return self;
}

// 1.13c D2CMP+0x01460: scalar deleting destructor over 0x1300.
uint32_t* d2cmp_1460(uint32_t* self, uint8_t flags, const D2CmpListEnv* e) {
    d2cmp_1300(self, e);
    if ((flags & 1) && self) e->smemFree(self, "delete", -1, 0);
    return self;
}

// 1.13c D2CMP+0x014D0: scalar deleting destructor over 0x13A0.
uint32_t* d2cmp_14d0(uint32_t* self, uint8_t flags, const D2CmpListEnv* e) {
    d2cmp_13a0(self, e);
    if ((flags & 1) && self) e->smemFree(self, "delete", -1, 0);
    return self;
}

}
