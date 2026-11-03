// 1.13c D2Client+0x7D850 (588 B): chat queue push. Stack a0 (ECX of 0xD378), a1 (byte stored at node+0x1A), ret 8.
// Node (0x26 B, links 32-bit as retail, the link dword at +0x22 is unaligned): +0..0x17 six line words, +0x18 u16 line count,
// +0x1A u32 a1, +0x1E u32 expiry (tick + 10000), +0x22 next node. Globals (retail RVAs, passed as the g[] words):
//   g[0]=0xDBC48 (screen height) g[1]=0x113AA4 queue head g[2]=0x113AA8 (u16 node count) g[3]=0x11BF34 bubble flag
//   g[4]=0x11C24C player record g[5]=0x11C250 g[6]=0x11C268 g[7]=0x11BBFC local player g[8]=0xFADE0 g[9]=0x113A74 vtable object
// Callees go through env->call(rva, eax, ecx, edx, s0, s1): 0xBF7A alloc (ECX 0x26, EDX file, 0xD42, 0), 0xD3A8 (ECX), 0xD378 (list, EDX = &count),
// 0xBF86 free (ECX node, EDX file, id, 0), 0x7B9C0 (EAX node), 0x7B330 (EAX next, ECX old head, EDX line sum), 0x87FE0 (EAX 6, 0), 0x87D80, 0x88350 (ECX &floats, EDX bubble).
// The consistency-failure exit (0xD68 message and exit(-1)) never returns in retail: env->fatal and return. The vtable branch (g[8] && g[9]) is env->vcall.
#include <cstdint>
#include <cstring>

struct D2Client7d850Env {
    uint32_t (*call)(uint32_t rva, uint32_t eax, uint32_t ecx, uint32_t edx, uint32_t s0, uint32_t s1);
    uint32_t (*tick)();
    void (*floats)(uint32_t edx_bubble, const uint32_t* f3);   // 0x88350 with its 3-dword block (logged by hookblk)
    void (*fatal)();
    void (*vcall)(uint32_t obj);
};

static inline uint32_t r7d_rd(uint32_t a) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
static inline uint32_t r7d_rd16(uint32_t a) { uint16_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 2); return v; }
static inline void r7d_wr(uint32_t a, uint32_t v) { std::memcpy((void*)(uintptr_t)a, &v, 4); }
static inline void r7d_wr16(uint32_t a, uint32_t v) { uint16_t w = (uint16_t)v; std::memcpy((void*)(uintptr_t)a, &w, 2); }

extern "C" void d2client_7d850_port(uint32_t a0, uint32_t a1, uint32_t* g, const D2Client7d850Env* env) {
    const uint32_t kFile = 0xB20D4BDCu;   // module string at +0xD4BDC (the retail 0x6FB84BDC), logged module-relative
    uint32_t node = env->call(0xBF7A, 0, 0x26, kFile, 0xD42, 0);
    for (uint32_t o = 0; o < 0x24; o += 4) r7d_wr(node + o, 0);
    r7d_wr16(node + 0x24, 0);
    r7d_wr(node + 0x22, 0);
    r7d_wr(node + 0x1E, env->tick() + 10000u);
    uint32_t token = env->call(0xD3A8, 0, 0xD, 0, 0, 0);
    int32_t count = 0;
    uint32_t esi = env->call(0xD378, 0, a0, (uint32_t)(uintptr_t)&count, g[0] - 0x46u, 0);
    r7d_wr16(node + 0x18, (uint32_t)count > 6u ? 6u : (uint32_t)count);
    if (count > 0) {
        for (int32_t i = 0; i < count; ++i) {
            const int32_t nw = (int32_t)r7d_rd16(node + 0x18);
            const uint32_t line = r7d_rd(esi);
            if (i < nw) r7d_wr(node + 4u * i, line);
            else env->call(0xBF86, 0, line, kFile, 0xD56, 0);
            const uint32_t next = r7d_rd(esi + 4);
            env->call(0xBF86, 0, esi, kFile, 0xD59, 0);
            esi = next;
        }
    }
    r7d_wr(node + 0x1A, a1 & 0xFFu);
    uint32_t head = g[1];
    if (head) {
        uint32_t tail = head, links = 0;
        while (r7d_rd(tail + 0x22)) { tail = r7d_rd(tail + 0x22); ++links; }
        if ((links & 0xFFFFu) != ((g[2] & 0xFFFFu) - 1u)) { env->fatal(); return; }
        r7d_wr(tail + 0x22, node);
    } else {
        g[1] = node;
    }
    env->call(0x7B9C0, node, 0, 0, 0, 0);
    head = g[1];
    g[2] = (g[2] & ~0xFFFFu) | ((g[2] + 1u) & 0xFFFFu);
    if (head) {
        uint32_t sum = 0;
        for (uint32_t p = head; p; p = r7d_rd(p + 0x22)) sum += r7d_rd16(p + 0x18);
        if ((int32_t)sum > 0x12) {
            const uint32_t nxt = r7d_rd(head + 0x22);
            g[2] = (g[2] & ~0xFFFFu) | ((g[2] - 1u) & 0xFFFFu);
            g[1] = nxt;
            env->call(0x7B330, nxt, head, sum, 0, 0);
        }
    }
    if (g[3]) {
        const uint32_t rec = (g[4] && (int32_t)g[5] > 6) ? g[4] + 0x354u : 0;
        const uint8_t flag = *(const uint8_t*)(uintptr_t)(rec + 0x3C);
        const uint32_t ebx = g[6];
        if (flag) {
            uint32_t b = 0;
            bool go = true;
            if (r7d_rd16(rec + 0x45)) b = env->call(0x87FE0, 6, 0, 0, 0, 0);
            if (!b) {
                b = env->call(0x87D80, 0, 0, 0, 0, 0);
                if (!b) go = false;
                else {
                    r7d_wr(b + 4, 6); r7d_wr(b + 0x38, 0); r7d_wr(b + 0x34, ebx); r7d_wr(b + 0x24, 0); r7d_wr(b + 0x1C, 0xFF);
                    *(uint8_t*)(uintptr_t)(b + 0x3C) = *(const uint8_t*)(uintptr_t)(rec + 0x4E);
                    const uint32_t f[3] = { 0, 0, 0x43A00000u };
                    env->floats(b, f);
                }
            }
            if (go && !g[7]) *(uint8_t*)(uintptr_t)(b + 0x3C) += 0x50;
        }
    }
    if (g[8] && g[9]) env->vcall(g[9]);
    env->call(0xD3A8, 0, token, 0, 0, 0);
}
