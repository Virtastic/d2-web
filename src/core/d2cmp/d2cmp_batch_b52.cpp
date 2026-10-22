// src/core/d2cmp/d2cmp_batch_b52.cpp -- alpha-lane batch: UCELLIST list teardown rows of 1.13c D2CMP.dll
// from the cached Ghidra decompiles. No spec, no vector. Guest pointers are 32-bit addresses; SEH frames
// dropped. Register-passed args (EDI/EBX/EAX) are taken as explicit parameters. Callees outside the batch
// and Storm #403 (SMemFree) come in through the env. Skipped: 0x4E66 (EBP/EAX frame spill, no contract),
// 0x7875 (CRT fatal-error box: GetModuleFileNameA + MessageBox + _exit), crt allocator rows.
#include <cstdint>

struct D2CmpB52Env {
    void (*f1580)(uint32_t* cell);               // 0x1580
    void (*f1490)(uint32_t* cell);               // 0x1490
    void (*f1170)(uint32_t* self);               // 0x1170
    void (*f16c0)(uint32_t* list);               // 0x16C0
    void (*f15d0)(uint32_t* cell);               // 0x15D0
    void (*smemFree)(void* p, const char* tag, int32_t line, uint32_t flags);  // Storm #403
};

static const char kChunk[]  = "__AV__TSHashObjectChunk_UCELLIST__";  // tag at 0x6FE34108
static const char kCell[]   = "__AUCELLIST__";                       // tag at 0x6FE3417C
static const char kExpl[]   = "__AV__TSExplicitList_UCELLIST__";     // tag at 0x6FE34144

// Unlink link pair {first, prev-or-~head} at lk.
static void unlink_pair(uint32_t* lk) {
    uint32_t first = lk[0], prev = lk[1];
    uint32_t* slot = ((int32_t)prev < 0) ? (uint32_t*)(uintptr_t)~prev
                                         : (uint32_t*)((uintptr_t)lk + (prev - ((uint32_t*)(uintptr_t)first)[1]));
    *slot = first;
    ((uint32_t*)(uintptr_t)lk[0])[1] = lk[1];
    lk[0] = 0;
    lk[1] = 0;
}

extern "C" {

// 1.13c D2CMP+0x1CA0 (EBX = object): drain the list headed at +8.
void d2cmp_b52_1ca0(uint32_t* self) {
    while ((int32_t)self[2] > 0) {
        uint32_t* n = (uint32_t*)(uintptr_t)self[2];
        if (n[0]) unlink_pair(n);
    }
}

// 1.13c D2CMP+0x1C10: per 0x38-byte cell run 0x16C0 on +0x2C then 0x15D0, then free the array.
void d2cmp_b52_1c10(uint32_t* self, const D2CmpB52Env* e) {
    for (uint32_t i = 0; i < self[1]; i++) {
        uint32_t* c = (uint32_t*)(uintptr_t)(self[2] + i * 0x38);
        e->f16c0(c + 0xb);
        e->f15d0(c);
    }
    if (self[2]) e->smemFree((void*)(uintptr_t)self[2], kCell, (int32_t)0xfffffffe, 0);
}

// 1.13c D2CMP+0x1FC0: unlink the pair at +0x10, then 0x1C10.
void d2cmp_b52_1fc0(uint32_t* self, const D2CmpB52Env* e) {
    if (self[4]) unlink_pair(self + 4);
    d2cmp_b52_1c10(self, e);
}

// 1.13c D2CMP+0x1F80 (EAX = object): 0x1580 per 0x38-byte cell, free the array.
void d2cmp_b52_1f80(uint32_t* self, const D2CmpB52Env* e) {
    for (uint32_t i = 0; i < self[1]; i++) e->f1580((uint32_t*)(uintptr_t)(self[2] + i * 0x38));
    if (self[2]) e->smemFree((void*)(uintptr_t)self[2], kCell, (int32_t)0xfffffffe, 0);
}

// 1.13c D2CMP+0x1AD0 (EAX = object): 0x1490 per 12-byte cell, free the array.
void d2cmp_b52_1ad0(uint32_t* self, const D2CmpB52Env* e) {
    for (uint32_t i = 0; i < self[1]; i++) e->f1490((uint32_t*)(uintptr_t)(self[2] + i * 0xc));
    if (self[2]) e->smemFree((void*)(uintptr_t)self[2], kExpl, (int32_t)0xfffffffe, 0);
}

// 1.13c D2CMP+0x1B70 (thiscall): run 0x1170; bit 0 of flags frees `this`.
uint32_t* d2cmp_b52_1b70(uint32_t* self, uint8_t flags, const D2CmpB52Env* e) {
    e->f1170(self);
    if ((flags & 1) && self) e->smemFree(self, "delete", -1, 0);
    return self;
}

// 1.13c D2CMP+0x1840 (EDI = object): free every chunk (+0x40) then every cell (+0x30).
void d2cmp_b52_1840(uint32_t* self, void (*f1fc0)(uint32_t*), const D2CmpB52Env* e) {
    int32_t h;
    while ((h = (int32_t)self[0x10]) > 0) {
        f1fc0((uint32_t*)(uintptr_t)h);
        e->smemFree((void*)(uintptr_t)h, kChunk, (int32_t)0xfffffffe, 0);
    }
    while ((h = (int32_t)self[0xc]) > 0) {
        e->f1580((uint32_t*)(uintptr_t)h);
        e->smemFree((void*)(uintptr_t)h, kCell, (int32_t)0xfffffffe, 0);
    }
}

// 1.13c D2CMP+0x1880 (EBX = object): drain 0x1F30, then walk the 12-byte bucket table at +0x1C.
// mode 0: call vtable[0](node) per entry; else unlink each node in place.
void d2cmp_b52_1880(uint32_t* self, int32_t mode, void (*f1f30)(uint32_t*)) {
    self[4] = 0;
    f1f30(self);
    for (uint32_t i = 0; i < self[6]; i++) {
        uint32_t* b = (uint32_t*)(uintptr_t)(self[7] + i * 0xc);
        for (int32_t off; (off = (int32_t)b[2]) > 0;) {
            if (mode == 0) {
                (*(void (**)(int32_t))(uintptr_t)*(uint32_t*)(uintptr_t)self[0])(off);
            } else {
                uint32_t* n = (uint32_t*)(uintptr_t)(b[0] + off);
                if (n[0]) unlink_pair(n);
            }
        }
    }
}

}
