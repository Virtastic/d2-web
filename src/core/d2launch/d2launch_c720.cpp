// 1.13c D2Launch+0xC720 (sub_C720). EAX = the game description (ASCII, may be 0); five stack words, callee pops them (ret 0x14):
// a0 = text copied to the node start (Ordinal_501, max 0x7fffffff), a1 = the raw wire record, a2 -> node+0x348, a3 -> node+0x340, a4 -> node+0x344.
// Builds a game-list node (0x350 bytes from the allocator), parses the wire record into it, creates the list control (Ordinal_10154) and links the node
// into the list DAT_6fa65ec8 (count DAT_6fa65ed0, dirty flag DAT_6fa65eb0). Returns 1; 0 when sub_9F40 reports the name a duplicate (node freed by sub_F3A0).
//  node+0x100 UTF-16 description (win2Unicode, 0xff max, or empty); +0x326 u16 at rec+0; +0x300/+0x310 two 11-byte blocks padded to 16 with 0xff;
//  +0x320 class byte - 1; +0x322 u8; +0x324 u16 flags; +0x328 u16; +0x32e/+0x32f/+0x330 bytes (0xff -> 0); +0x32a three chars; +0x334 := 0; +0x338 control handle;
//  +0x34c next. Control type: 5; flags&4 set and flags&8 clear: 1 when Ordinal_10227() != 0 or class < 5; flags&4 and flags&8: class 0,1,6,9 -> 8, else 9 (type 5).
//  Handle 0 retries with a zeroed +0x300, +0x301 = 1, class 7, type 5; a second 0 is fatal (id 0x4d3, exit(-1)), which the port reports through `fail` and then
//  leaves. Link: Ordinal_10227() == 0 appends at the tail; otherwise inserted before the first node whose (+0x344, +0x340) is not above (a4, a3).
#include <cstdint>
#include <cstring>

struct D2lC720S {
    uint32_t (*alloc)(uint32_t size, uint32_t file, uint32_t line);        // 0x97DE (ECX size, EDX file string, stack line/0)
    void (*copy)(uint32_t dst, uint32_t src, uint32_t max);                // 0x9790 Ordinal_501
    void (*w2u)(uint32_t dst, uint32_t src, uint32_t max);                 // IAT 0x1C038 D2Lang win2Unicode
    void (*u2w)(uint32_t dst, uint32_t src, uint32_t max);                 // IAT 0x1C03C D2Lang unicode2Win
    uint32_t (*rd16)(uint32_t p);                                          // 0x9856 Ordinal_10085
    uint32_t (*f9f40)(uint32_t name);                                      // 0x9F40, EBX = wide name
    void (*f3a0)(uint32_t buf, uint32_t node);                             // 0xF3A0, EAX = ascii name, ECX = node
    void (*cpy3)(uint32_t dst, uint32_t src, uint32_t n);                  // 0x13B0 strncpy
    uint32_t (*f980e)();                                                   // 0x980E Ordinal_10227
    uint32_t (*mk)(uint32_t cls, uint32_t type, uint32_t p300, uint32_t p310);   // 0x99CA Ordinal_10154
    void (*fail)(uint32_t id);                                             // 0x9820 + 0x97F6 + 0x165B exit(-1)
    void (*f9a0c)(uint32_t h, uint32_t z);
    void (*f9a06)(uint32_t h, uint32_t z);
    void (*f9a2a)(uint32_t h, uint32_t five);
    void (*fb600)();
};
struct D2lC720G { uint32_t* head; int32_t* count; uint32_t* flag; };

static inline uint32_t c720_r32(uint32_t a) { uint32_t v; memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
static inline void c720_w32(uint32_t a, uint32_t v) { memcpy((void*)(uintptr_t)a, &v, 4); }
static inline void c720_w16(uint32_t a, uint32_t v) { uint16_t x = (uint16_t)v; memcpy((void*)(uintptr_t)a, &x, 2); }
static inline uint8_t c720_r8(uint32_t a) { return *(const uint8_t*)(uintptr_t)a; }
static inline void c720_w8(uint32_t a, uint32_t v) { *(uint8_t*)(uintptr_t)a = (uint8_t)v; }
static inline uint32_t c720_r16(uint32_t a) { uint16_t v; memcpy(&v, (const void*)(uintptr_t)a, 2); return v; }

static uint32_t c720_block(const D2lC720S* s, uint32_t node, uint32_t dst, uint32_t p) {
    (void)s; (void)node;
    int i = 0;
    do { c720_w8(dst + (uint32_t)i, c720_r8(p)); p++; i++; } while (i < 11);
    for (; i < 16; i++) c720_w8(dst + (uint32_t)i, 0xff);
    return p;
}

extern "C" int32_t d2l_c720(uint32_t desc, uint32_t a0, uint32_t rec, uint32_t a2, uint32_t a3, uint32_t a4,
                            const D2lC720G* g, uint32_t modbase, const D2lC720S* s) {
    uint32_t n = s->alloc(0x350, modbase + 0x1eb84, 0x429);
    c720_w32(n + 0x340, a3);
    c720_w32(n + 0x344, a4);
    c720_w32(n + 0x348, a2);
    s->copy(n, a0, 0x7fffffff);
    uint32_t nm = n + 0x100;
    if (desc) s->w2u(nm, desc, 0xff); else c720_w16(nm, 0);
    c720_w16(n + 0x326, s->rd16(rec));
    uint32_t p = rec + 2;
    if (s->f9f40(nm)) {
        char buf[0x100];
        s->u2w((uint32_t)(uintptr_t)buf, nm, 0xff);
        s->f3a0((uint32_t)(uintptr_t)buf, n);
        return 0;
    }
    p = c720_block(s, n, n + 0x300, p);
    c720_w8(n + 0x320, c720_r8(p) - 1); p++;
    p = c720_block(s, n, n + 0x310, p);
    c720_w32(n + 0x34c, 0);
    c720_w16(n + 0x322, c720_r8(p)); p++;
    c720_w16(n + 0x324, s->rd16(p)); p += 2;
    c720_w16(n + 0x328, s->rd16(p)); p += 2;
    for (uint32_t o = 0x32e; o <= 0x330; o++) {
        uint8_t b = c720_r8(p); p++;
        c720_w8(n + o, b == 0xff ? 0 : b);
    }
    s->cpy3(n + 0x32a, p, 3);
    uint32_t flags = c720_r16(n + 0x324);
    uint32_t cls = c720_r8(n + 0x320), type = 5;
    if (flags & 4) {
        if (!(flags & 8)) {
            if (s->f980e() != 0 || c720_r8(n + 0x320) < 5) type = 1;
        } else {
            cls = (cls == 0 || cls == 1 || cls == 6 || cls == 9) ? 8 : 9;
            type = 5;
        }
    }
    uint32_t h = s->mk(cls & 0xff, type, n + 0x300, n + 0x310);
    c720_w32(n + 0x338, h);
    if (h == 0) {
        for (int i = 0; i < 16; i++) { c720_w8(n + 0x300 + (uint32_t)i, 0); c720_w8(n + 0x310 + (uint32_t)i, 0xff); }
        c720_w8(n + 0x301, 1);
        h = s->mk(7, 5, n + 0x300, n + 0x310);
        c720_w32(n + 0x338, h);
    }
    if (c720_r32(n + 0x338) == 0) { s->fail(0x4d3); return 0; }
    c720_w32(n + 0x334, 0);
    s->f9a0c(h, 0);
    h = c720_r32(n + 0x338);
    s->f9a06(h, 0);
    s->f9a2a(c720_r32(n + 0x338), 5);
    uint32_t head = *g->head;
    if (head) {
        if (s->f980e() == 0) {
            uint32_t t = head;
            while (c720_r32(t + 0x34c)) t = c720_r32(t + 0x34c);
            c720_w32(t + 0x34c, n);
        } else {
            uint32_t prev = 0, cur = head;
            while (cur) {
                uint32_t k = c720_r32(cur + 0x344);
                if (a4 > k) break;
                if (a4 == k && a3 > c720_r32(cur + 0x340)) break;
                prev = cur;
                cur = c720_r32(cur + 0x34c);
            }
            if (prev) {
                c720_w32(n + 0x34c, c720_r32(prev + 0x34c));
                c720_w32(prev + 0x34c, n);
                goto linked;
            }
            c720_w32(n + 0x34c, head);
            *g->head = n;
        }
    } else {
        *g->head = n;
    }
linked:
    *g->count += 1;
    s->fb600();
    *g->flag = 1;
    return 1;
}
