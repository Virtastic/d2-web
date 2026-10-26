// src/core/d2sound/d2sound_batch_z40.cpp -- 1.13c D2Sound.dll stage-A batch z40.
// Imports (Ordinal_258/255/257/10102/10103, critical section 6f9c6530, vtable calls, fatal-assert
// Ordinal_10265/10024+_exit) are host seams. 32-bit links are uint32_t.
#include <cstdint>

struct Z40Seams {
    void (*enter)();                                  // EnterCriticalSection(6f9c6530)
    void (*leave)();                                  // LeaveCriticalSection(6f9c6530)
    void (*fatal)(uint32_t line);        // Ordinal_10265(line)+Ordinal_10024+_exit(-1)
    int (*ord258)(uint32_t h, uint32_t* a, uint8_t* b);
    int (*ord255)(void* self, uint32_t a, uint32_t flags, uint32_t pos, uint32_t c, uint32_t d, uint32_t e);
    void (*ord257)(uint32_t h);
    int (*ord10102)();
    void (*ord10103)();
    int (*vtLock)(uint32_t obj);                      // (**(vtbl+0x30))(obj,0,0,1)
    void (*vtRelease)(uint32_t obj);                  // (**(vtbl+0x48))(obj)
    uint32_t (*alloc)(uint32_t size, uint32_t flag);  // Ordinal_10042
    void (*sub7BC0)();
    int (*regRead)(const char*, const char*, uint32_t, uint32_t*);   // Ordinal_423
    void (*regWrite)(const char*, const char*, uint32_t, uint32_t);  // Ordinal_426
};
// Sound object, 32-bit layout.
struct Z40Snd { uint32_t p0, p4, p8[5], f1c, f20[5], f30, f34[5], f48; /* +0x40 at word 16 */ };
static inline uint32_t& z40_w(void* o, int off) { return *(uint32_t*)((uint8_t*)o + off); }
struct Z40Opts { uint32_t mixer, master, music, bias, npc, optMusic; };
struct Z40Globals { uint32_t* chan[8]; uint32_t cnt65b8, h65c0, f65c4; };  // 6f9c6560.., 65b8, 65c0, 65c4

// 1.13c D2sound+0x6850
uint32_t z40_6850(const Z40Seams& s, void* o) {
    if (!o) s.fatal(0x310);
    s.enter();
    uint32_t v; uint8_t b[4];
    if (z40_w(o, 0x48) && s.ord258(z40_w(o, 0x48), &v, b)) { s.leave(); return v >> 2; }
    s.leave();
    return 0;
}

// 1.13c D2sound+0x8690  (singly linked list length, next at +4)
int z40_8690(const uint8_t* const* base, const uint8_t* (*next)(const uint8_t*), const uint8_t* n) {
    (void)base; int c = 0;
    for (; n; n = next(n)) c++;
    return c;
}

// 1.13c D2sound+0x8710  (push new 0xd4 node {data=param_2, next=old head})
void z40_8710(const Z40Seams& s, uint32_t* head, uint32_t data) {
    uint32_t n = s.alloc(0xd4, 0);
    uint32_t* p = (uint32_t*)(uintptr_t)n;
    p[0] = 0; p[1] = 0;
    uint32_t old = *head; p[0] = data; p[1] = old; *head = n;
}

// 1.13c D2sound+0x80A0  (any of the 8 channels in use; fatal if a slot is null / has no handle)
int z40_80A0(const Z40Seams& s, const Z40Globals& g) {
    if (!g.cnt65b8) return 0;
    for (int i = 0; i < 8; i++) {
        uint32_t* p = g.chan[i];
        if (!p || !p[0]) s.fatal(0x2a5);
        if (p[7]) return 1;
    }
    return 0;
}

// 1.13c D2sound+0x6F30
int z40_6F30(const Z40Seams& s, uint32_t* o) {
    if (!o) s.fatal(0x279);
    s.enter();
    if (s.vtLock(o[0]) < 0) { s.leave(); return 0; }
    o[7] = 1;
    s.leave();
    return 1;
}

// 1.13c D2sound+0x6820
void z40_6820(const Z40Seams& s, uint8_t* o, uint32_t v) {
    if (!o) s.fatal(0x32f);
    z40_w(o, 0x40) = v;
}

// 1.13c D2sound+0x66F0  (__thiscall this, a=param_1, b=param_2)
int z40_66F0(const Z40Seams& s, void* self, int a, int b) {
    if (!self) s.fatal(0x3db);
    if (!s.ord10102()) return 0;
    s.enter();
    uint32_t u = z40_w(self, 4);
    if (s.ord255(self, u, b ? 0x40000u : 0u, (uint32_t)a * 4, 0x7fffffff, 0x7fffffff, z40_w(self, 0))) {
        z40_w(self, 0x48) = u; z40_w(self, 0x1c) = 1; z40_w(self, 0x30) = 0xffffffff;
        s.leave();
        return 1;
    }
    s.ord10103();
    s.leave();
    return 0;
}

// 1.13c D2sound+0x6680
void z40_6680(const Z40Seams& s, uint32_t* o) {
    if (!o || !o[0]) s.fatal(0x400);
    s.enter();
    if (o[0x12]) {
        s.ord257(o[0x12]);
        s.vtRelease(o[0]);
        s.ord10103();
        o[0x12] = 0; o[7] = 0;
    }
    s.leave();
}

// 1.13c D2sound+0x6950
uint32_t z40_6950(const Z40Seams& s, const uint32_t* o) {
    if (!o || !o[0]) s.fatal(0x2a5);
    return o[7];
}

// 1.13c D2sound+0x7D00
void z40_7D00(const Z40Seams& s, Z40Globals& g, uint32_t v) {
    if (v && !g.h65c0) { s.sub7BC0(); g.f65c4 = 1; }
    g.h65c0 = v;
}
