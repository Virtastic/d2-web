// 1.13c D2Common batch 81bc0: +0x81BC0, +0x4D010, +0x452C0, +0x3D810, +0x36CD0, +0x83CA0, +0x84890.
// Implementation lane (no spec/vector yet): register-convention internals written over named structs that carry
// only the fields the decompile touches (retail offsets in comments). Fatal paths ending in _exit(-1) return, as
// elsewhere in src/core/d2common. Callees not yet ported are weak externs named by RVA.
// ponytail: structs are minimal local views; replace with the typed engine structs when a row gets a vector.
#include <cstdint>
#include <cstdlib>
#include <cstring>

struct B81Grid { int32_t* data; int32_t* rowOff; };                 // cell (r,c) = data[rowOff[r] + c]
struct B81Dims { char _0[0x3c]; int32_t w; int32_t h; };            // +0x3c, +0x40

extern "C" {
__attribute__((weak)) void D2Common_817A0(void* self, int x, int y, int arg);
__attribute__((weak)) void D2Common_4CA90(void* a, int flag, int count, int zero);   // register order as decompiled
__attribute__((weak)) void D2Common_3BA60(void* room, void* outRect);
__attribute__((weak)) void D2Common_44A30();
__attribute__((weak)) void D2Common_45240();
__attribute__((weak)) void D2Common_445A0();
__attribute__((weak)) void D2Common_5BE50(void* self, const void* p);
__attribute__((weak)) void D2Common_36840(int a, void* b);
__attribute__((weak)) void D2Common_36B10(int a, int b);
__attribute__((weak)) int  D2Common_834D0(int x, int y, void* ctx, void* ent, void* p2);
__attribute__((weak)) int  D2Common_837F0(int x, int y, void* ctx, void* ent);   // y register-passed in retail
__attribute__((weak)) void D2Common_84220(int v, int x, int y, void* ctx, void* ent, void* p2, int extra);
extern void (*D2Common_EB244[])();   // line-plot callbacks at 0x6FDEB244 (register args, not recovered)
}

struct B81Flood {   // esi
    B81Dims* dims;      // [0]
    B81Grid* visited;   // [1]
    void* _2; void* _3;
    B81Grid* tiles;     // [4]
    void* _5;
    int32_t counter;    // [6]
    uint32_t mark;      // [7]
};

// 1.13c D2Common+0x81BC0: over cells (0..w, 0..h) whose visited flag 0x10000000 is clear: bump the counter,
// mark = counter&0x0FFFFFFF | 0x10000000 (| 0x30000000 if the tile masks to 0x1E00000 under 0x1E0FF00 or is
// negative), then 817A0(self, x, y, -1).
void D2Common_81BC0_FloodCells(B81Flood* s)
{
    const int w = s->dims->w, h = s->dims->h;
    for (int y = 0; y <= h; ++y)
        for (int x = 0; x <= w; ++x) {
            if ((uint32_t)s->visited->data[s->visited->rowOff[y] + x] & 0x10000000) continue;
            const uint32_t n = (uint32_t)s->counter++ + 1;
            s->mark = (n & 0xFFFFFFF) | 0x10000000;
            const uint32_t t = (uint32_t)s->tiles->data[s->tiles->rowOff[y] + x];
            if ((t & 0x1E0FF00) == 0x1E00000 || (int32_t)t < 0) s->mark = (n & 0xFFFFFFF) | 0x30000000;
            D2Common_817A0(s, x, y, -1);
        }
}

struct B4DEntry { char _0[8]; struct { void* a; int32_t na; void* b; int32_t nb; void* c; int32_t nc; }* span; char _10[0x18]; int32_t flag; }; // +8, +0x20
struct B4DOwner { B4DEntry** entries; char _[0x20 - sizeof(void*)]; void* grid; int32_t count; };   // [0], [8], [9]

// 1.13c D2Common+0x4D010: given the owner (param_1) and a room, size and zero a collision buffer
// (Collisn.cpp:0x185), store it at owner[8], then for every entry stamp its three spans (+8/+0xC,
// +0/+4, +0x10/+0x14) with the entry flag via 4CA90. A null entry is fatal (0x2C5/0xB1/0xBE) -> return.
// The buffer allocation/size comes from 3BA60's rect (w*h*2 bytes zeroed after a 0x24-byte header); the
// allocator ordinal 10045 args are not fully recovered by the decompiler, so calloc stands in.
void D2Common_4D010_BuildCollisionStamps(B4DOwner* o, void* room)
{
    if (!room) return;
    int32_t rect[10] = {};
    D2Common_3BA60(room, rect);
    const int32_t cells = rect[7] * rect[6];   // local_14 * local_18
    o->grid = std::calloc(1, 0x24 + (size_t)cells * 2);
    for (int i = 0; i < o->count; ++i) {
        B4DEntry* e = o->entries[i];
        if (!e) return;
        auto* s = e->span;
        const int f = e->flag;
        D2Common_4CA90(s ? s->b : nullptr, f, s ? s->nb : 0, 0);   // span +8/+0xC (null span -> 0,0)
        D2Common_4CA90(s->a, f, s->na, 0);                          // +0/+4
        D2Common_4CA90(s->c, f, s->nc, 0);                          // +0x10/+0x14
    }
}

struct B452Room {
    char _0[8]; int32_t* rooms;     // +8 (array, resized below)
    char _1[0x28 - 8 - sizeof(void*)];
    uint32_t flags;                 // +0x28 (layout approximated: only logical order matters here)
    int32_t count; B452Room* next;
    int32_t x, y, w, h;
};

// 1.13c D2Common+0x452C0: walk the linked list from `head` (eax) via +0x24. For nodes whose flags (+0x28) have
// bit (level+4): if the register arg param_5 != -1, call 44A30 then 45240 and return 1. Otherwise measure the
// gap between the node's and the room's [x,y] extents (+0x34/+0x38 pos, +0x3C/+0x40 size); when both gaps
// are < 6, grow room's neighbour array (+0x2C count, +8 array; DrlgRoom.cpp:0x314) appending the node, call
// 445A0, result 1. Allocation failure is fatal (0x315) -> return. Returns 0 if nothing matched.
// ponytail: the array grow uses realloc (ordinal 10047 old pointer is a register arg in retail).
int D2Common_452C0_LinkNearbyRoom(void* headv, uint8_t level, void* roomv, int param_5)
{
    struct N { char _[0x24]; N* next; uint32_t flags; int32_t _2c; };
    int ret = 0;
    // Offsets are retail; node/room are raw buffers.
    auto I = [](void* p, int o) -> int32_t& { return *(int32_t*)((char*)p + o); };
    for (char* n = (char*)headv; n; n = *(char**)(n + 0x24)) {
        if (!(*(uint32_t*)(n + 0x28) & (1u << ((level + 4) & 0x1f)))) continue;
        if (param_5 != -1) { D2Common_44A30(); D2Common_45240(); return 1; }
        int dx = I(roomv, 0x34) < I(n, 0x34) ? (I(n, 0x34) - I(roomv, 0x3c)) - I(roomv, 0x34)
                                             : (I(roomv, 0x34) - I(n, 0x3c)) - I(n, 0x34);
        int dy = I(roomv, 0x38) < I(n, 0x38) ? (I(n, 0x38) - I(roomv, 0x40)) - I(roomv, 0x38)
                                             : (I(roomv, 0x38) - I(n, 0x40)) - I(n, 0x38);
        if (dx < 6 && dy < 6) {
            const int c = ++I(roomv, 0x2c);
            void** arr = (void**)std::realloc(*(void**)((char*)roomv + 8), (size_t)c * sizeof(void*));
            *(void***)((char*)roomv + 8) = (void**)arr;
            if (!arr) return ret;          // fatal 0x315
            arr[c - 1] = n;
            D2Common_445A0();
            ret = 1;
        }
    }
    return ret;
}

// 1.13c D2Common+0x3D810: copy the 3x3 neighbourhood of cell (row, col) of grid g into out[9] (esi). Order as
// retail: out[0..2] = row+1/row/row-1 at col-1 ... i.e. out[2]=(r-1,c-1) out[1]=(r,c-1) out[0]=(r+1,c-1)
// out[5]=(r-1,c) out[4]=(r,c) out[3]=(r+1,c) out[8]=(r-1,c+1) out[7]=(r,c+1) out[6]=(r+1,c+1).
void D2Common_3D810_Read3x3(const B81Grid* g, int row, int col, int32_t out[9])
{
    auto at = [&](int r, int c) { return g->data[g->rowOff[r] + c]; };
    out[2] = at(row - 1, col - 1); out[1] = at(row, col - 1); out[0] = at(row + 1, col - 1);
    out[5] = at(row - 1, col);     out[4] = at(row, col);     out[3] = at(row + 1, col);
    out[8] = at(row - 1, col + 1); out[7] = at(row, col + 1); out[6] = at(row + 1, col + 1);
}

struct B36Line { int32_t x, y; void* _2; void* _3; int32_t** end; };   // [0],[1],[4]->{x,y}
struct B36Rect { int32_t x, y, w, h; };

// 1.13c D2Common+0x36CD0: Bresenham from line start (x,y) to *end, plotting a nThick-long run per step
// (perpendicular to the major axis) through callback table entry `sel` (0x6FDEB244) for cells inside rect.
void D2Common_36CD0_PlotLine(const B36Line* l, const B36Rect* r, int sel, int nThick)
{
    auto inRect = [](const B36Rect* r, int x, int y) { return r->x <= x && r->y <= y && x < r->w + r->x && y < r->h + r->y; };
    int x = l->x, y = l->y;
    int dx = l->end[0][0] - x, dy = l->end[0][1] - y, sx = 1, sy = 1;
    if (dx < 0) { dx = -dx; sx = -1; }
    if (dy < 0) { dy = -dy; sy = -1; }
    int err = 0;
    auto plot = [&](int px, int py) { if (inRect(r, px, py)) D2Common_EB244[sel](); };   // args in regs, not recovered
    if (dx < dy) {
        for (int i = 0; i < nThick; ++i) plot(x + i, y);
        for (int n = dy; n > 0; --n) {
            y += sy; err += dx;
            if (dy < err) { x += sx; err -= dy; }
            for (int i = 0; i < nThick; ++i) plot(x + i, y);
        }
    } else {
        for (int i = 0; i < nThick; ++i) plot(x, y + i);
        for (int n = dx; n > 0; --n) {
            x += sx; err += dy;
            if (dx < err) { y += sy; err -= dx; }
            for (int i = 0; i < nThick; ++i) plot(x, y + i);
        }
    }
}

// 1.13c D2Common+0x83CA0: lazily build the cached preset descriptor at param+0x50. If null: 5BE50(this, param+4)
// fills it; if its +0x4C is null that is fatal 0x2B6 -> return. Then for each of the +0x14 entries call
// 36840(0, entry[+0x2C+4i]) and 36840(0, entry[+0x1C+4i]); for i in 1..count-1 call 36B10(i<<18, 0); and
// 36840(0, +0x3C) if +0x18 is set, 36840(0, +0x44) if +0x44 is set.
void D2Common_83CA0_InitPresetDescriptor(void* self, char* param)
{
    char** slot = (char**)(param + 0x50);
    if (*slot) return;
    D2Common_5BE50(self, param + 4);
    char* d = *slot;
    if (!d || !*(void**)(d + 0x4c)) return;       // fatal 0x2B6
    auto P = [](char* b, int o) { return *(void**)(b + o); };
    const int n = *(int32_t*)(d + 0x14);
    for (int i = 0; i < n; ++i) { D2Common_36840(0, P(d, 0x2c + 4 * i)); D2Common_36840(0, P(*slot, 0x1c + 4 * i)); d = *slot; }
    for (int i = 1; i < *(int32_t*)(*slot + 0x14); ++i) D2Common_36B10(i << 18, 0);
    if (*(int32_t*)(*slot + 0x18)) D2Common_36840(0, P(*slot, 0x3c));
    if (*(int32_t*)(*slot + 0x44)) D2Common_36840(0, P(*slot, 0x44));
}

// 1.13c D2Common+0x84890: scatter preset objects over a room grid. Context A = *ctx: +0x3C/+0x40 dims,
// +0x14/+0x18 the 64-bit seed (seed = lo*0x6AC690C5 + hi, 64-bit store, low half used), +0x58->+0x1B4->+0x478
// the value passed to 84220. P = *(param_2+0x50): P[0] mode, P[0x13] entry count, P[0x14] entries (0x18
// stride; +8 w, +0xC h, +0x14 n). Mode 1: every interior cell passing 834D0 -> 84220(...,0). Mode 2: every cell
// passing 837F0 whose seeded roll % 100 exceeds param_2[+0x11C + ctx[0xD]*4] -> 84220(..., (w+1)*(roll+1)),
// roll = seed&(n-1) for power-of-two n, seed % n otherwise, 0 for n<1 (no seed step).
void D2Common_84890_ScatterObjects(char** ctx, char* param2)
{
    auto seedStep = [](char* A) { uint64_t s = (uint64_t) * (uint32_t*)(A + 0x14) * 0x6ac690c5u + *(uint32_t*)(A + 0x18); std::memcpy(A + 0x14, &s, 8); return (uint32_t)s; };
    char* A = ctx[0];
    const int v = *(int32_t*)(*(char**)(*(char**)(A + 0x58) + 0x1b4) + 0x478);
    char* P = *(char**)(param2 + 0x50);
    const int w0 = *(int32_t*)(A + 0x3c), h0 = *(int32_t*)(A + 0x40);
    for (int i = 0; i < *(int32_t*)(P + 0x4c); ++i) {
        char* e = *(char**)(P + 0x50) + i * 0x18;
        const int w = w0 - *(int32_t*)(e + 8) + 1, h = h0 - *(int32_t*)(e + 0xc) + 1;
        if (w <= 0 || h <= 0) continue;
        if (*(int32_t*)P == 1) {
            for (int y = 1; y < h; ++y) for (int x = 1; x < w; ++x)
                if (D2Common_834D0(x, y, ctx, e, param2)) D2Common_84220(v, x, y, ctx, e, param2, 0);
        } else if (*(int32_t*)P == 2) {
            for (int y = 0; y < h; ++y) for (int x = 0; x < w; ++x) {
                if (!D2Common_837F0(x, y, ctx, e)) continue;
                const int chance = *(int32_t*)(param2 + 0x11c + *(int32_t*)((char*)ctx + 0x34) * 4);
                if (chance >= (int)(seedStep(A) % 100)) continue;
                int32_t n = *(int32_t*)(e + 0x14); uint32_t r = 0;
                if (n > 0) r = (n & (n - 1)) == 0 ? ((uint32_t)(n - 1) & seedStep(A)) : seedStep(A) % (uint32_t)n;
                D2Common_84220(v, x, y, ctx, e, param2, (*(int32_t*)(e + 8) + 1) * (int)(r + 1));
            }
        }
    }
}
