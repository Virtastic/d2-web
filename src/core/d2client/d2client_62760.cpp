// 1.13c D2Client+0x62760 -- Four stack words: a panel id, a selector and a pair. The dword +0 of the current node (pointer 0x11C1C4) is remembered, or -1 with none. D2Common ordinal 10749 (thunk 0xCC3A, ECX = the id) gives a record whose dword +8 goes to 0x62710 in EAX; that answers the layout. By id: 0x28 sets layout dword +4 to 1 and lays 4 rows of 5 cells, x step 0xA0, y step 0x64, x bias 0x35, y bias -0x51, numbering from 0 when the selector is 1 and from 0x14 otherwise, skipping the numbers 0, 10, 15, 16, 19, 20, 25, 30, 35, 36 and 39 (the list at 0xD2DB8); 0x67 sets 2 and lays 2 rows of 2, steps 0x88 and 0x5A, biases -3 and -0x32; 0x6D sets 3 and lays 2 rows of 3, steps 0xB4 and 0xAA, biases -0x14 and -0x9B; any other id halts retail. D2Common ordinal 10375 (thunk 0xCC58) is given pointers to the pair and may rewrite it; each half is divided by 10 toward zero. The first cell sits at x = pair x / 10 - (x step * columns / 2 + x bias), y = pair y / 10 - y step * rows / 2 - y bias. Per cell that is not skipped (x, y and the number must each fit a signed word, else retail halts): 0x5F6B0 gives a node, its dword +0 is cleared, the words +4, +6, +8 take the number, x and y, and 0x61320 runs with ECX = the node and EDX = the layout + 0x14. Cells step x within a row and y between rows; the number counts every cell, skipped or not. Last, with a remembered id: the list at 0x11C1C0 (next at +0x18) is searched for a node with that id; with none a 0x1C-byte node is taken from Fog ordinal 10042 (thunk 0xBF7A, ECX = 0x1C, EDX = the string 0xD537C, stack 0x5ED and 0), zeroed, given the id and pushed on the list head. A node other than the current one runs 0x61140 and 0x5F300, becomes current, and 0x61E70 runs; its answer is returned. Otherwise the row count is returned.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2client_11c1c0;
__attribute__((weak)) uint32_t g_d2client_11c1c4;
static inline uint32_t gd(const uint8_t* a, uint32_t off) { uint32_t v; __builtin_memcpy(&v, a + off, 4); return v; }
static inline void gw(uint8_t* a, uint32_t off, uint32_t v) { __builtin_memcpy(a + off, &v, 4); }
namespace d2cl {
uint32_t f62760(GE& e, uint32_t id, uint32_t a1, uint32_t a2, uint32_t a3)
{
    const uint32_t cur0 = g_d2client_11c1c4;
    const uint32_t prev = cur0 ? e.rd32(cur0) : 0xffffffffu;
    const uint32_t rec = e.h(0xCC3A, 0, id);
    const uint32_t lay = e.h(0x62710, e.rd32(rec + 8));
    static const int32_t SKIP[] = { 0, 10, 15, 16, 19, 20, 25, 30, 35, 36, 39, -1 };
    int32_t xs, ys, bx, by, rows, cols, n = 0;
    const int32_t* skip = nullptr;
    if (id == 0x28) { e.wr32(lay + 4, 1); xs = 0xa0; ys = 0x64; bx = 0x35; by = -0x51; rows = 4; cols = 5; skip = SKIP; n = a1 != 1 ? 0x14 : 0; }
    else if (id == 0x67) { e.wr32(lay + 4, 2); xs = 0x88; ys = 0x5a; bx = -3; by = -0x32; rows = 2; cols = 2; }
    else if (id == 0x6d) { e.wr32(lay + 4, 3); xs = 0xb4; ys = 0xaa; bx = -0x14; by = -0x9b; rows = 2; cols = 3; }
    else return 0;   // retail halts here
    uint32_t px = a2, py = a3;
    e.h(0xCC58, 0, 0, 0, 0, 0, e.stk(&px), e.stk(&py));
    const int32_t cx = (int32_t)px / 10, cy = (int32_t)py / 10;
    const int32_t x0 = xs * cols / 2 + bx;
    int32_t y = cy - ys * rows / 2 - by;
    for (int32_t r = 0; r < rows; ++r) {
        int32_t x = cx - x0;
        for (int32_t c = 0; c < cols; ++c) {
            bool sk = false;
            if (skip) for (const int32_t* p = skip; *p != -1; ++p) if (*p == n) { sk = true; break; }
            if (!sk) {
                if (x < -0x8000 || x > 0x7fff || y < -0x8000 || y > 0x7fff || n < -0x8000 || n > 0x7fff) return 0;   // retail halts here
                const uint32_t nd = e.h(0x5F6B0);
                e.wr32(nd, 0);
                e.wr8(nd + 4, (uint32_t)n); e.wr8(nd + 5, (uint32_t)n >> 8);
                e.wr8(nd + 6, (uint32_t)x); e.wr8(nd + 7, (uint32_t)x >> 8);
                e.wr8(nd + 8, (uint32_t)y); e.wr8(nd + 9, (uint32_t)y >> 8);
                e.h(0x61320, 0, nd, lay + 0x14);
            }
            x += xs; ++n;
        }
        y += ys;
    }
    if (prev != 0xffffffffu) {
        uint32_t nd = g_d2client_11c1c0;
        while (nd && e.rd32(nd) != prev) nd = e.rd32(nd + 0x18);
        if (!nd) {
            nd = e.h(0xBF7A, 0, 0x1c, 0xB20D537Cu, 0, 0, 0x5ed, 0);
            for (uint32_t k = 0; k < 0x1c; k += 4) e.wr32(nd + k, 0);
            e.wr32(nd, prev);
            e.wr32(nd + 0x18, g_d2client_11c1c0);
            g_d2client_11c1c0 = nd;
        }
        if (nd != g_d2client_11c1c4) {
            e.h(0x61140);
            e.h(0x5F300);
            g_d2client_11c1c4 = nd;
            return e.h(0x61E70);
        }
    }
    return (uint32_t)rows;
}
}
