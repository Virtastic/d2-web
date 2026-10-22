// Prior art: none; written from the cached 1.13c decompile. Offsets are 1.13c (32-bit) layout, as in the sibling batches.
// MAPPED-quality: 6E470 6E4E0 70D60. Implemented but arg/index GUESSED (leave NAMED): 6EDB0 (table index by unit ptr is a decompile oddity), 6F520 (EAX/ECX/EDX roles).
// decompile, left NAMED): 6E680 6E930 70010 70190 70240 70BF0 70CF0.
#include <cstdint>
#include <cstdlib>

typedef uint8_t u8;
// unverified seams (D2Common/D2Game imports by ordinal or D2Game internals)
extern "C" {
uint32_t D2C_11067(void* ev);                      // Ordinal_11067: event frame
void D2C_10422(void* a, void* ev);                 // Ordinal_10422
void D2C_10158(void* unit);                        // Ordinal_10158
void* D2C_10331(void* unit);                        // Ordinal_10331
void D2C_11090(void* unit, int mode);              // Ordinal_11090: SetUnitMode
void* D2C_10688(int cls);                          // Ordinal_10688: monstats row
int D2C_11044(void* a, int b);                     // Ordinal_11044
void D2GAME_AEA80(void* game, void* unit, int type, int frame, int a, int b, int c); // 0x8EA80 event enqueue
int D2GAME_8B570(void* st);                        // 0x6B570
int D2GAME_21560(void* p, int n);                  // 0x1560
void* D2GAME_24950(int id, int a, void* game, void* x, int b, int c, int d); // 0x24950
void D2GAME_23CA0(void* game, void* p, int v);     // 0x23CA0
void D2GAME_248A0(int a, void* game);              // 0x248A0
void D2C_10826(void* x);
void D2C_10408(void* unit);                        // Ordinal_10408
void D2GAME_6DD60(int v);                          // 0x6DD60
void D2GAME_6E680(void* thisv, void* game, uint32_t id, short kind); // 0x6E680 (args guessed, see header)
void D2GAME_6A1A0(void* unit);                     // 0x6A1A0
void* D2GAME_E03A0(void* p, uint32_t v);             // 0xE03A0
}

static inline u8* PT(u8* p, int off) { return *(u8**)(p + off); }
static inline int& I(u8* p, int off) { return *(int*)(p + off); }
static uint32_t Rnd(u8* seed) { uint64_t* s = (uint64_t*)seed; uint32_t lo = (uint32_t)*s, hi = *(uint32_t*)(seed + 4); uint64_t v = (uint64_t)lo * 0x6ac690c5u + hi; *s = v; return (uint32_t)v; }

// 1.13c D2Game+0x6E470
void D2GAME_6E470(u8* game, u8* unit)
{
    if (!unit) return;
    u8* ev = PT(unit, 0xa4);
    if (!ev) return;
    uint32_t f = D2C_11067(ev);
    if (f <= (uint32_t)I(game, 0xa8)) {
        D2C_10422(PT(game, 0x1c), ev);
        *(u8**)(unit + 0xa4) = nullptr;
        D2C_10158(unit);
        I(unit, 0xc4) |= 0x100;
        return;
    }
    D2GAME_AEA80(game, unit, 6, f, 0, 0, 0);
}

// 1.13c D2Game+0x6E4E0
void D2GAME_6E4E0(u8* game, u8* unit)
{
    short mode = 0;
    if (unit) {
        mode = (short)I(unit, 0x10);
        if (I(unit, 4) == 399) {
            uint32_t m = (uint32_t)I(unit, 0x10) & 0xffff;
            if (m != 0 && m != 2) return;
            D2C_10331(unit);
            uint32_t r = Rnd(PT(game, 0x10f0));
            D2GAME_AEA80(game, unit, 8, (int)(r % 0xfa) + 0x19 + I(game, 0xa8), 0, 0, 0);
            D2C_11090(unit, 1);
            u8* ms = (u8*)D2C_10688(I(unit, 4));
            D2GAME_AEA80(game, unit, 1, (int)(*(uint32_t*)(ms + 0xdc) >> 8) + 1 + I(game, 0xa8), 0, 0, 0);
            return;
        }
    }
    int d = D2C_11044(PT(game, 0xbc), 0);
    if (d == 0) {
        if (mode == 2 || mode == 1) D2C_11090(unit, 0);
        D2GAME_AEA80(game, unit, 8, I(game, 0xa8) + 1000, 0, 0, 0);
        return;
    }
    if (d > 0 && d < 4) {
        if (mode == 0) {
            D2C_11090(unit, 1);
            u8* ms = (u8*)D2C_10688(unit ? I(unit, 4) : -1);
            D2GAME_AEA80(game, unit, 1, (int)(*(uint32_t*)(ms + 0xdc) >> 8) + 1 + I(game, 0xa8), 0, 0, 0);
        }
        D2GAME_AEA80(game, unit, 8, I(game, 0xa8) + 1000, 0, 0, 0);
        return;
    }
    D2GAME_AEA80(game, unit, 8, I(game, 0xa8) + 600, 0, 0, 0);
}

// 1.13c D2Game+0x6EDB0  (param_1 = AI/ctx struct: [0]=game [1]=unit [3]=seed ptr [4]=class)
uint32_t D2GAME_6EDB0(int* c)
{
    u8* ctx = (u8*)c;
    u8* unit = PT(ctx, 4);
    if ((!unit || !I(unit, 0x10)) && D2GAME_8B570(c)) {
        D2C_10331(*(void**)(ctx + 4));
        D2C_10826(*(void**)(ctx + 4));
        D2C_11090(PT(ctx, 4), 1);
        unit = PT(ctx, 4);
        if (unit) I(unit, 0xc4) &= ~2;
        u8* game = PT(ctx, 0);
        u8* ms = (u8*)D2C_10688(I(ctx, 0x10));
        D2GAME_AEA80(game, PT(ctx, 4), 1, (int)(*(uint32_t*)(ms + 0xdc) >> 8) + 1 + I(game, 0xa8), 0, 0, 0);
        uint32_t r = Rnd(PT(ctx, 0xc));
        if (r % 0xff <= *(uint32_t*)(ms + 0x194)) {
            u8* tbl = PT(game, 0xf0 + I(ctx, 0x10) * 4);  // decompile indexes by class (iVar5)
            unsigned n = tbl[0x10];
            short* sp = (short*)(tbl + 0x14);
            for (unsigned i = 0; i < n; i++, sp += 0x1a) {
                short id = *sp;
                if (D2GAME_21560(sp, 2)) {
                    void* x = D2C_10331(PT(ctx, 4));
                    void* m = D2GAME_24950(id, 0, game, x, 0, 0, -1);
                    if (m) { D2GAME_23CA0(game, m, 1); D2GAME_248A0(0, game); }
                    break;
                }
            }
        }
        if (ms[0x121] == 0) D2C_10408(PT(ctx, 4));
        D2GAME_6DD60(PT(PT(ctx, 4), 0x14)[4] & 0x7f);
    }
    return 1;
}

// 1.13c D2Game+0x6F520  (usercall: EAX = game (guess), ECX = unit, EDX = ai ctx/param_2)
void D2GAME_6F520(u8* game, u8* unit, u8* ai)
{
    if (unit) {
        short k = (short)I(unit, 4);
        if (k == 0x50 || k == 0x5a) {
            D2GAME_6E680(ai ? (void*)(uintptr_t)I(ai, 0xc) : (void*)(uintptr_t)-1, game, (uint32_t)I(unit, 0xc), 2);
        } else if (k == 0x151) {
            D2GAME_6A1A0(unit);
            I(ai, 0x64) = -1; I(ai, 0x68) = 6; ai[0x6c] = 0;
            return;
        } else if (k == 0x98) {
            D2C_11090(unit, 0);
            I(ai, 0x64) = -1; I(ai, 0x68) = 6; ai[0x6c] = 0;
            return;
        }
    }
    I(ai, 0x64) = -1; I(ai, 0x68) = 6; ai[0x6c] = 0;
}

// 1.13c D2Game+0x70D60
uint32_t D2GAME_70D60(u8* self, void* p)
{
    if (self[0x6c] != 0 && I(self, 0x68) == 0 && D2GAME_E03A0(p, (uint32_t)I(self, 0x64))) return 1;
    return 0;
}
