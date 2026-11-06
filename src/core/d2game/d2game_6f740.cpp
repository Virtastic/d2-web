// 1.13c D2Game+0x6F740, injectable callees. ECX = game, ESI = missile unit; stack word 0 = the send target handed to 0x8AA10, stack word 1 =
// "client path" flag (ret 8). Builds the 0x20-byte 0x73 packet in a zeroed stack block and sends it (0x8AA10: EAX = block, 1 stack word).
// Fields (byte offsets): 0 = 0x73, 5 word B0EA(unit), 7 dword 0x1100, 0xB dword 0x1130, 0xF/0x13 dwords AAC6/AAD8(unit+0x2C) when flag,
// 0x17 word B102, 0x19 byte [rec], 0x1A dword [rec+0xC], 0x1E byte B0FC, 0x1F byte A42A(unit,0x148,0). rec = 0xE03A0 (EAX = unit+0x94,
// ECX = game, EDX = unit+0x98). Gates: type 3, flag bit 10 of +0xC8, rec non-null, level index in sgptDataTables (+0xB64/+0xB6C, stride 0x1A4),
// gdwBitMasks byte +0x14 & rec byte +4.
#include <cstdint>
#include <cstring>
struct G6f740Env {
    uint32_t (*e03a0)(uint32_t eax, uint32_t ecx, uint32_t edx);
    uint32_t (*b0ea)(uint32_t unit);
    uint32_t (*f1100)(uint32_t ecxUnit);
    uint32_t (*f1130)(uint32_t ecxUnit);
    uint32_t (*aac6)(uint32_t v);
    uint32_t (*aad8)(uint32_t v);
    uint32_t (*b102)(uint32_t unit);
    uint32_t (*b0fc)(uint32_t unit);
    uint32_t (*a42a)(uint32_t unit, uint32_t id, uint32_t z);
    void (*send)(uint32_t eaxBlock, const uint32_t* block, uint32_t w0);
};
static uint32_t rd32(uint32_t a, uint32_t off) { uint32_t v; memcpy(&v, (const void*)(uintptr_t)(a + off), 4); return v; }
// 1.13c D2Game+0x6F740
void g_6f740(uint32_t ecx, uint32_t esi, uint32_t w0, uint32_t w1, uint32_t dtp, uint32_t bm, const G6f740Env& env)
{
    if (esi == 0 || rd32(esi, 0) != 3) return;
    if (((rd32(esi, 0xC8) >> 10) & 1) == 0) return;
    const uint32_t rec = env.e03a0(rd32(esi, 0x94), ecx, rd32(esi, 0x98));
    if (rec == 0) return;
    const int32_t idx = (int32_t)rd32(esi, 4);
    if (idx < 0) return;
    const uint32_t dt = rd32(dtp, 0);
    if (idx >= (int32_t)rd32(dt, 0xB6C)) return;
    const uint32_t row = (uint32_t)idx * 0x1A4u + rd32(dt, 0xB64);
    if (row == 0) return;
    if (((rd32(bm, 0x14) & 0xFF) & (rd32(row, 4) & 0xFF)) == 0) return;
    uint32_t blk[8] = {};
    uint8_t* b = (uint8_t*)blk;
    auto w16 = [&](uint32_t o, uint32_t v) { b[o] = (uint8_t)v; b[o + 1] = (uint8_t)(v >> 8); };
    auto w32 = [&](uint32_t o, uint32_t v) { memcpy(b + o, &v, 4); };
    b[0] = 0x73;
    w16(5, env.b0ea(esi));
    w32(7, env.f1100(esi));
    w32(0xB, env.f1130(esi));
    if (w1 != 0) {
        w32(0xF, env.aac6(rd32(esi, 0x2C)));
        w32(0x13, env.aad8(rd32(esi, 0x2C)));
    }
    w16(0x17, env.b102(esi));
    const uint32_t r4 = rd32(rec, 0), rc = rd32(rec, 0xC);
    b[0x19] = (uint8_t)r4;
    w32(0x1A, rc);
    b[0x1E] = (uint8_t)env.b0fc(esi);
    b[0x1F] = (uint8_t)env.a42a(esi, 0x148, 0);
    env.send((uint32_t)(uintptr_t)blk, blk, w0);
}
