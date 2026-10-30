// 1.13c D2Common batch 44a30: +0x44A30, +0x45240, +0x834D0, +0x84220, +0x837F0.
// Env-seam style: callees are function pointers on Env, register args are explicit params.
// All struct accesses use retail 32-bit byte offsets on retail-layout images; a stored "pointer" is
// read as u32 and widened to uintptr_t (no pointer is ever narrowed). Implementations only; unvectored.
#include <cstdint>
typedef uintptr_t P;
static inline int32_t  rd(P b, uint32_t o) { return *(const int32_t*)(b + o); }
static inline uint32_t ru(P b, uint32_t o) { return *(const uint32_t*)(b + o); }
static inline P        rp(P b, uint32_t o) { return (P)ru(b, o); }
static inline void     wr(P b, uint32_t o, uint32_t v) { *(uint32_t*)(b + o) = v; }

struct D2CommonBatch44A30Env {
    P    (*alloc10047)(int size, const char* file, int line, int zero);   // Ordinal_10047
    P    (*alloc10045)(const char* file, int line, int zero);             // Ordinal_10045
    void (*fatal)(int line);                                              // 10265/10024/_exit(-1); may return
    void (*f445A0)(P room);                                               // sub_445A0
    void (*f69950)(P a, int b);                                           // sub_69950
    uint32_t (*f6C5D0)(uint32_t ecx, int c);                              // sub_6C5D0
    void (*f6FD0)();                                                      // sub_67FD0 (0x84220), no arguments
    uint32_t (*f68B90)(P ctx, int n);                                     // sub_68B90
    void (*f68CD0)(P ctx, int x, P z, int y, uint32_t v);                // sub_68CD0
    void (*f83630)(P a, uint32_t b, P c);                                 // sub_83630
    void (*cb250)();                                                      // PTR_FUN_6fdeb250 indirect
};

// 1.13c D2Common+0x44A30 : room (EAX) grows its +8 array by one dword (count at +0x2c).
void D2Common_44A30(const D2CommonBatch44A30Env& e, P room)
{
    int n = rd(room, 0x2c) + 1;
    wr(room, 0x2c, n);
    P a = e.alloc10047(n * 4, "..\\Source\\D2Common\\DRLG\\DrlgRoom.cpp", 0x314, 0);
    wr(room, 8, (uint32_t)a);
    if (!a) { e.fatal(0x315); return; }
    wr(a, (uint32_t)(n * 4 - 4), 0x314);   // guess: decompiler shows the line constant as stored value
    e.f445A0(room);
}

// 1.13c D2Common+0x45240 : prepend a new 0x18-byte node to list at EDI+0x4c; ESI = payload.
void D2Common_45240(const D2CommonBatch44A30Env& e, P esi, P edi, uint32_t ecxAfter69950)
{
    P n = e.alloc10045("..\\Source\\D2Common\\DRLG\\DrlgRoom.cpp", 0x322, 0);
    for (uint32_t o = 0; o < 0x18; o += 4) wr(n, o, 0);
    wr(n, 0, (uint32_t)esi);
    e.f69950(esi, rd(rp(edi, 0x58), 0x1d0));
    (void)ecxAfter69950;   // guess: ECX is extraout from 69950
    wr(n, 0x10, e.f6C5D0(ecxAfter69950, 'b'));
    wr(n, 8, 1);
    wr(n, 4, ru(edi, 0x4c));
    wr(edi, 0x4c, (uint32_t)n);
}

// 1.13c D2Common+0x834D0 : 1 if the rect p4 {x,y,w,h} placed at (p1,p2) is free in p3, else 0.
int D2Common_834D0(int p1, int p2, P p3, P p4, P p5)
{
    int h = rd(p4, 12), w = rd(p4, 8);
    for (int j = 0, row = p2 * 4; j < h; j++, row += 4)
        for (int i = 0; i < w; i++) {
            int idx = (rd(rp(p5, 0xf8), (rd(p4, 4) + j) * 4) + rd(p4, 0) + i) * 4;
            bool hit = (*(uint8_t*)(rp(p5, 0xf4) + idx) & 2) != 0;
            if (!hit && rd(p5, 0xac))
                hit = (*(uint8_t*)(rp(p5, 0xa4) + idx) & 1) != 0;   // same index expr in decompile
            if (!hit) continue;
            P g = rp(p3, 0x24);
            uint32_t v = ru(rp(g, 0), (rd(rp(g, 4), row) + i + p1) * 4);
            if ((v & 0x3f0ff00) || !(v & 2)) return 0;
            for (int k = 0; k < rd(p3, 0x2c); k++) {
                P q = rp(p3, 0x14 + k * 4);
                if (q && (ru(rp(q, 0), (rd(rp(q, 4), row) + i + p1) * 4) & 1)) return 0;
            }
        }
    return 1;
}

// 1.13c D2Common+0x84220 : stamp rect p5 of room p6 into p4's layers via cb250, then 83630.
void D2Common_84220(const D2CommonBatch44A30Env& e, uint32_t p1, int p2, int p3, P p4, P p5, P p6, int p7)
{
    int layers = rd(rp(p6, 0x50), 0x14);
    P ctx = rp(p4, 0);
    int x0 = rd(p5, 0) + p7;
    e.f6FD0();    // sub_67FD0 takes no arguments (1.13c call site)
    for (int j = 0; j < rd(p5, 12); j++)
        for (int i = 0; i < rd(p5, 8); i++) {
            int rowOff = (rd(p5, 4) + j) * 4;
            uint32_t f = rd(rp(p6, 0x50), 0x18) ? ru(rp(p6, 0xf4), (rd(rp(p6, 0xf8), rowOff) + i + x0) * 4) : 0;
            if (f & 2) e.cb250();
            P lay = p6 + 0x58; P cur = p4;
            for (int l = 0; l < layers; l++, lay += 20, cur += 4) {
                // piVar9 indexes dwords of the retail image: [-1],[0],[0x13],[0x14]
                if ((ru(rp(lay, 0x4c), (rd(rp(lay, 0x50), rowOff) + i + x0) * 4) & 1) && rd(cur, 20)) e.cb250();
                if (rd(rp(lay, (uint32_t)-4), (rd(rp(lay, 0), rowOff) + i + x0) * 4) && rd(cur, 4)) e.cb250();
            }
            if (ru(rp(p6, 0x108), (rd(rp(p6, 0x10c), rowOff) + i + x0) * 4) & 0x8000000) {
                int y = rd(ctx, 0x38), x = rd(ctx, 0x34);
                uint32_t v = e.f68B90(ctx, 0xd);
                e.f68CD0(ctx, x + i + p2, 0, y + j + p3, v);
            }
        }
    e.f83630(p4, p1, p6);
}

// 1.13c D2Common+0x837F0 : EAX = row offset base; 1 if rect p3 of room EDI matches layers of p2.
int D2Common_837F0(int eax, P edi, int p1, P p2, P p3)
{
    for (int j = 0; j < rd(p3, 12); j++) {
        int row = (rd(p3, 4) + j) * 4, ro = (eax + j) * 0 + eax * 4 + j * 4;
        for (int i = 0; i < rd(p3, 8); i++) {
            int x = rd(p3, 0) + i;
            P a = rp(p2, 4);
            if (rd(rp(edi, 0x54), (rd(rp(edi, 0x58), row) + x) * 4) !=
                rd(rp(a, 0), (rd(rp(a, 4), ro) + i + p1) * 4)) return 0;
            uint32_t u = rd(rp(edi, 0x50), 0x18) ? ru(rp(edi, 0xf4), (rd(rp(edi, 0xf8), row) + x) * 4) : 0;
            if (u & 2) {
                P g = rp(p2, 0x24);
                uint32_t v = ru(rp(g, 0), (rd(rp(g, 4), ro) + i + p1) * 4);
                if (!(v & 2) || ((v ^ u) & 0x3f0ff00)) return 0;
            }
            u = rd(rp(edi, 0x50), 0x14) ? ru(rp(edi, 0xa4), (rd(rp(edi, 0xa8), row) + x) * 4) : 0;
            if (u & 1) {
                P g = rp(p2, 0x14);
                uint32_t v = ru(rp(g, 0), (rd(rp(g, 4), ro) + i + p1) * 4);
                if (!(v & 1) || ((v ^ u) & 0x3f0ff00)) return 0;
            }
        }
    }
    return 1;
}
