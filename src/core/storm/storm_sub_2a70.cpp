// 1.13c Storm+0x02A70 -- bounded chain walk over 12-byte entries; resets/calls Storm+0x2DD0 when chain exceeds 13 hops.
#include <cstdint>
struct S2A70Ent { uint32_t base; uint32_t next; int32_t link; };
struct S2A70Ctx { uint8_t p0[0x10]; uint32_t hops; uint8_t p14[8]; S2A70Ent* ents; uint8_t p20[4]; uint32_t count; };
static int g_2dd0_calls;  // seam: Storm+0x2DD0 unmapped here
static void Seam2DD0_2A70(S2A70Ctx*) { g_2dd0_calls++; }

// ctx in ECX, idx in EAX (retail register convention)
uint32_t Storm_2A70(S2A70Ctx* ctx, uint32_t idx) {
    if (ctx->count > 0x3fe) return 0;
    ctx->hops = ctx->hops < 4 ? 0 : ctx->hops - 3;
    int32_t cur = ctx->ents[idx].link;
    if (cur < 1) cur = 0;
    for (;;) {
        if (cur < 1) return 0;
        if (++ctx->hops > 0xd) break;
        cur = *(int32_t*)((uintptr_t)ctx->ents[idx].base + (uint32_t)cur + 4);
    }
    ctx->hops = 0;
    Seam2DD0_2A70(ctx);
    return 1;
}
