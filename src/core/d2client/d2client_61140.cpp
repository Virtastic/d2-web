// 1.13c D2Client+0x61140 (470 B): load the four automap level tables. No arguments.
// p := [11C1C4]; p == 0 returns. The local block is {p[0], p[1], [[11C3BC]+4], n0..n3, a0..a3}; n_i := F3B0(p[2+i]) (6 per
// flag-0 node of the tree); for each non-zero n_i the table is allocated (bf7a, file D537C, lines 4EA/4F1/4F8/4FF), [11C1B4] := it and
// F360(tree) fills it; then 60510(EBX = block); then each allocation that exists is freed (bf86, lines 509..50C).
#include <cstdint>
typedef uint32_t u32;
struct D2c61140Env {
    void (*f3b0)(u32 tree, u32* out);    // ECX = tree, EDX = &out (accumulates)
    void (*f360)(u32 tree);              // ECX = tree
    u32 (*alloc)(u32 line);              // bf7a, EDX = file, stack (line, 0)
    void (*release)(u32 ptr, u32 line);  // bf86, ECX = ptr, EDX = file, stack (line, 0)
    void (*f60510)(u32* blk);            // EBX = block
};
// 1.13c D2Client+0x61140
void d2c_61140(u32 g_c1c4, u32 g_c3bc, u32* g_c1b4, const D2c61140Env* e) {
    if (!g_c1c4) return;
    const u32* p = (const u32*)(uintptr_t)g_c1c4;
    u32 b[11] = {};
    b[0] = p[0]; b[1] = p[1]; b[2] = ((const u32*)(uintptr_t)g_c3bc)[1];
    for (int i = 0; i < 4; i++) e->f3b0(p[2 + i], &b[3 + i]);
    static const u32 aline[4] = {0x4ea, 0x4f1, 0x4f8, 0x4ff};
    for (int i = 0; i < 4; i++) {
        if (!b[3 + i]) continue;
        b[7 + i] = e->alloc(aline[i]);
        *g_c1b4 = b[7 + i];
        e->f360(p[2 + i]);
    }
    e->f60510(b);
    for (int i = 0; i < 4; i++)
        if (b[7 + i]) e->release(b[7 + i], 0x509 + i);
}
