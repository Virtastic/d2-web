// Fog batch z47: intrusive link-list unlink/insert and heap-slot helpers (+0x1080..+0x1630).
// Decompile-derived from build-wasm/ghidra-fog/decomp. 32-bit links are uint32_t addresses resolved
// through fog_z47_base (seam; null = addresses are host-truncated and must be mapped by the caller).
#include <cstdint>

extern "C" {
uint8_t* fog_z47_base __attribute__((weak));
void* (*fog_z47_alloc)(uint32_t size, const char* tag, int32_t line, uint32_t flags) __attribute__((weak));  // Ordinal_401
void (*fog_z47_free)(uint32_t p, const char* tag, int32_t line, uint32_t flags) __attribute__((weak));       // Ordinal_403
void (*fog_z47_heap_fix)(uint32_t heap, uint32_t idx) __attribute__((weak));  // Fog+0x1520
}

namespace {
uint32_t& W(uint32_t a) { return *(uint32_t*)((fog_z47_base ? fog_z47_base : (uint8_t*)0) + a); }

void unlink_node(uint32_t p) {  // inlined body shared by +0x1080/+0x1170/+0x1310/+0x1350/+0x11F0
    uint32_t n = W(p);
    if (!n) return;
    uint32_t l = W(p + 4);
    uint32_t t = (int32_t)l < 0 ? ~l : p + (l - W(n + 4));
    W(t) = n;
    W(W(p) + 4) = W(p + 4);
    W(p) = 0;
    W(p + 4) = 0;
}
}  // namespace

void fog_z47_1080(uint32_t p) { unlink_node(p); unlink_node(p); }  // 1.13c FOG+0x01080

void fog_z47_1120(uint32_t self) {  // 1.13c FOG+0x01120
    uint32_t h = self + 4;
    W(h) = 0; W(self + 8) = 0;
    W(h) = h; W(self) = 0; W(self + 8) = ~h;
}

static void drain_1350(uint32_t list) {  // 1.13c FOG+0x01350 (list in EBX)
    for (;;) { uint32_t p = W(list + 8); if ((int32_t)p <= 0) break; unlink_node(p); }
}
void fog_z47_1350(uint32_t list) { drain_1350(list); }

void fog_z47_1170(uint32_t self) { drain_1350(self); unlink_node(self + 4); }  // 1.13c FOG+0x01170

// 1.13c FOG+0x011F0: this=anchor (0 -> list+4), node=param_1 (0 -> list+4), mode 1 = insert before anchor
// else after. PARTIAL: node==0 case uses `list` (in_EAX) as retail's register arg.
void fog_z47_11F0(uint32_t list, uint32_t self, uint32_t node, int32_t mode) {
    uint32_t n = node ? node : list + 4;
    unlink_node(n);
    if (!self) self = list + 4;
    if (mode != 1) {
        uint32_t f = W(self);
        W(n) = f; W(n + 4) = W(f + 4); W(f + 4) = node; W(self) = n;
        return;
    }
    W(n) = self; W(n + 4) = W(self + 4);
    uint32_t u = W(self + 4), e = W(list);
    if ((int32_t)u < 0) { W(~u) = n; W(self + 4) = node; return; }
    if ((int32_t)e < 0) e = self - W(W(self) + 4);
    W(u + e) = n; W(self + 4) = node;
}

// 1.13c FOG+0x01290: alloc node (size param_2+0x24, tag "AUMessage LogManager", flags|8), zero links,
// insert via 11F0 if EDI(anchor, guessed = param `mode`) nonzero. PARTIAL: EDI/ECX are undeclared regs.
uint32_t fog_z47_1290(uint32_t list, uint32_t self, uint32_t size, uint32_t flags, int32_t mode) {
    if (!fog_z47_alloc) return 0;
    uint32_t p = (uint32_t)(uintptr_t)fog_z47_alloc(size + 0x24, "\\AUMessage\\LogManager", -2, flags | 8);
    if (!p) return 0;
    W(p) = 0; W(p + 4) = 0;
    if (mode) fog_z47_11F0(list, 0, p, mode);
    (void)self;
    return p;
}

void fog_z47_1310(uint32_t p) { unlink_node(p); }  // 1.13c FOG+0x01310 (EAX arg)

void fog_z47_13B0(uint32_t self) {  // 1.13c FOG+0x013B0
    if (W(self + 8) && fog_z47_free) fog_z47_free(W(self + 8), "\\PAX", -2, 0);
}

void fog_z47_1400(uint32_t self) {  // 1.13c FOG+0x01400
    for (uint32_t i = 0; i < W(self + 4); i++) {
        uint32_t e = W(W(self + 8) + i * 4) + W(self + 0x10);
        W(e + 4) = 0; W(e + 8) = 0;
    }
    if (W(self + 8) && fog_z47_free) fog_z47_free(W(self + 8), "\\PAX", -2, 0);
}

void fog_z47_1630(uint32_t self) { fog_z47_1400(self); }  // 1.13c FOG+0x01630

// 1.13c FOG+0x01390: vtable store + heap_fix(this[1]). PARTIAL: vtable const PTR_LAB_6ff78388 not stored.
void fog_z47_1390(uint32_t self) { if (W(self + 4) && fog_z47_heap_fix) fog_z47_heap_fix(W(self + 4), 0); }

// 1.13c FOG+0x01520: PARTIAL, heap remove-at-index (sift-down) needs 17D0/1790 and vtable compares;
// only the slot clear is implemented here.
void fog_z47_1520(uint32_t heap, uint32_t idx) {
    uint32_t e = W(W(heap + 8) + idx * 4) + W(heap + 0x10);
    W(e + 4) = 0; W(e + 8) = 0;
}
