// Storm register-arg batch b62. Retail 32-bit layouts: fields are int32, guest "pointers"
// are 32-bit values converted via G(). Hidden register inputs are explicit leading params.
#include <cstdint>
typedef int32_t i32; typedef uint32_t u32; typedef int16_t i16; typedef uint16_t u16; typedef uint8_t u8;
static inline i32* G(i32 v) { return (i32*)(uintptr_t)(u32)v; }
static inline i32 H(const void* p) { return (i32)(u32)(uintptr_t)p; }

// 1.13c Storm+0x02690  __fastcall(ecx unused, edx=table), hidden esi=key
extern "C" u32* storm_sub_2690(u32 /*ecx*/, i32* table /*edx*/, u32 key /*esi*/) {
    i32 bucket = table[7] + (i32)(key & (u32)table[9]) * 0xc;
    i32 head = *(i32*)(uintptr_t)(u32)(bucket + 8);
    i32 p = head < 1 ? 0 : head;
    while (p >= 1) {
        if (*(u32*)(uintptr_t)(u32)p == key) return (u32*)(uintptr_t)(u32)p;
        p = *(i32*)(uintptr_t)(u32)(p + *(i32*)(uintptr_t)(u32)bucket + 4);
    }
    return nullptr;
}

// 1.13c Storm+0x04390  __thiscall(ecx=this, stack node, mode), hidden eax=list
// Unlink node from its current list, then insert at head of `this` (or after `this` if mode==1).
extern "C" void storm_sub_4390(i32* list /*eax*/, i32* self /*ecx*/, i32* node, i32 mode) {
    i32* n = node ? node : list + 1;
    i32 nx = n[0];
    if (nx != 0) {
        u32 pv = (u32)n[1];
        i32* link = ((i32)pv < 0) ? G((i32)~pv) : G(H(n) + (i32)(pv - (u32)G(nx)[1]));
        *link = nx;
        G(n[0])[1] = n[1];
        n[0] = 0; n[1] = 0;
    }
    if (!self) self = list + 1;
    if (mode != 1) {
        i32 s0 = self[0];
        n[0] = s0; n[1] = G(s0)[1];
        G(s0)[1] = H(node);
        self[0] = H(n);
        return;
    }
    n[0] = H(self); n[1] = self[1];
    u32 pv = (u32)self[1];
    i32 off = list[0];
    if ((i32)pv < 0) { *G((i32)~pv) = H(n); self[1] = H(node); return; }
    if (off < 0) off = H(self) - G(self[0])[1];
    *(i32*)(uintptr_t)(u32)(pv + (u32)off) = H(n);
    self[1] = H(node);
}

// 1.13c Storm+0x025F0  (key stack, p2, p3), hidden edi=table. 2a70 and vtable[1] passed in.
extern "C" void storm_sub_25F0(i32* table /*edi*/, u32 key, i32 p2, i32 p3,
                               i32 (*sub_2A70)(i32*), u32* (*vt1)(i32 slot, i32, i32)) {
    u32 mask = (u32)table[9];
    if (sub_2A70(table) != 0) mask = (u32)table[9];
    u32* e = vt1(table[7] + (i32)(key & mask) * 0xc, p2, p3);
    i32* n = e ? G(table[1] + H(e)) : table + 2;
    if (n[0] != 0) {
        u32 pv = (u32)n[1];
        i32* link = ((i32)pv < 0) ? G((i32)~pv) : G(H(n) + (i32)(pv - (u32)G(n[0])[1]));
        *link = n[0];
        G(n[0])[1] = n[1];
        n[0] = 0; n[1] = 0;
    }
    i32 h = table[2];
    n[0] = h; n[1] = G(h)[1];
    G(h)[1] = H(e);
    table[2] = H(n);
    *e = key;
}

// 1.13c Storm+0x1D0B0  hidden eax=src, ebx=out_ptr, esi=out_hdr(0x12 bytes); sub_6868 = u16 strlen
extern "C" void storm_sub_1D0B0(i32 src /*eax*/, i32* pOut /*ebx*/, u8* hdr /*esi*/,
                                u16* o1, u16* o2, u16* o3, i32* o4, i32* o5,
                                i32 (*sub_6868)(const i16*)) {
    u8* s = (u8*)(uintptr_t)(u32)src;
    *(u32*)hdr = *(u32*)(s + 8);  *(u32*)(hdr + 4) = *(u32*)(s + 0xc);
    *(u32*)(hdr + 8) = *(u32*)(s + 0x10); *(u32*)(hdr + 0xc) = *(u32*)(s + 0x14);
    *(u16*)(hdr + 0x10) = *(u16*)(s + 0x18);
    u32 t = *(u32*)hdr; *(u32*)hdr = *(u32*)(hdr + 4); *(u32*)(hdr + 4) = t;
    i16* p;
    i16 v = *(i16*)(s + 0x1a);
    if (v == 0 || v == -1) p = (i16*)(s + 0x1c);
    else p = (i16*)(s + 0x1c + sub_6868((i16*)(s + 0x1a)) * 2);
    if (*p == 0 || *p == -1) *pOut = H(p + 1);
    else *pOut = H(p + sub_6868(p) + 1);
    if (*hdr & 0x40) {
        i32 base = *pOut;
        i32 n = sub_6868((i16*)(uintptr_t)(u32)base);
        i32 q = base + 2 + n * 2;
        *o1 = *(u16*)(uintptr_t)(u32)q;
        *o2 = *(u16*)(uintptr_t)(u32)(q + 2);
        *o3 = *(u8*)(uintptr_t)(u32)(q + 4) & 1;
        *o4 = q + 6;
        *o5 = *o4 + 2 + sub_6868((i16*)(uintptr_t)(u32)(q + 6)) * 2;
        return;
    }
    *o1 = 0; *o2 = 0; *o3 = 0; *o4 = 0;
    *o5 = *pOut + 2 + sub_6868((i16*)(uintptr_t)(u32)*pOut) * 2;
}

// 1.13c Storm+0x04FC0  (p1 unused, size stack, flags stack), hidden edi=list. Allocates size+0x238
// tagged ".?AUPROVIDERINFO@@" (line -2, flags|8), zeroes its first two dwords, links it via 4390.
extern "C" u32* storm_sub_4FC0(u32 /*p1*/, i32 size, u32 flags, i32* list /*edi*/,
                               u32* (*alloc_1C580)(u32, const char*, i32, u32),
                               void (*link_4390)(i32* node, i32* list)) {
    u32* p = alloc_1C580((u32)(size + 0x238), ".?AUPROVIDERINFO@@", -2, flags | 8);
    if (p) { p[0] = 0; p[1] = 0; }
    if (list) link_4390((i32*)p, list);
    return p;
}
