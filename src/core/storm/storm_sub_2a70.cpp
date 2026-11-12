// 1.13c Storm+0x02A70 -- bounded chain walk over 12-byte entries; resets/calls Storm+0x2DD0 when chain exceeds 13 hops.
// Retail's context is a 32-bit struct: the entry table at +0x1c is a dword (a host pointer field there would be
// 8-aligned to +0x20 under wasm64 and read the wrong word), and each entry is {base, next, link} in dwords.
#include <cstdint>
struct S2A70Ent { uint32_t base; uint32_t next; int32_t link; };
struct S2A70Ctx { uint8_t p0[0x10]; uint32_t hops; uint8_t p14[8]; uint32_t ents; uint8_t p20[4]; uint32_t count; };
static_assert(sizeof(S2A70Ent) == 12, "12-byte entries");
static_assert(__builtin_offsetof(S2A70Ctx, ents) == 0x1c && __builtin_offsetof(S2A70Ctx, count) == 0x24, "retail layout");
static int g_2dd0_calls;  // seam: Storm+0x2DD0 unmapped here
static void Seam2DD0_2A70(S2A70Ctx*) { g_2dd0_calls++; }

// ctx in ECX, idx in EAX (retail register convention)
uint32_t Storm_2A70(S2A70Ctx* ctx, uint32_t idx) {
    if (ctx->count > 0x3fe) return 0;
    ctx->hops = ctx->hops < 4 ? 0 : ctx->hops - 3;
    S2A70Ent* ents = (S2A70Ent*)(uintptr_t)ctx->ents;
    int32_t cur = ents[idx].link;
    if (cur < 1) cur = 0;
    for (;;) {
        if (cur < 1) return 0;
        if (++ctx->hops > 0xd) break;
        cur = *(int32_t*)(uintptr_t)(ents[idx].base + (uint32_t)cur + 4);
    }
    ctx->hops = 0;
    Seam2DD0_2A70(ctx);
    return 1;
}
