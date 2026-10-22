// Batch 9eeb0: D2Game 1.13c functions transcribed from the cached decompile.
// Guessed inputs (register-only args in retail): 9EEB0 unaff_ESI=table/ctx, retaddr+0x1c=caller frame;
//   A2800 in_EAX=unit-ish ptr, unaff_EBX=flag; A3090 unaff_EBX=unit, unaff_EDI=event rec;
//   A34E0 unaff_EBX=game; A35D0 in_EAX=game; A3AE0 in_EAX=unit.
// Pointers: retail 32-bit pointers are NOT ported as host pointers. Struct access is by byte
// offset on opaque buffers, and every ordinal/internal callee is an injected hook (no link deps).
// Fatal asserts (_exit(-1) paths) are reported by return/assert, not exit.
#include <cassert>
#include <cstdint>
#include <cstring>

namespace d2game_batch_9eeb0 {

template <class T> static T rd(const void* p, int off) { T v; std::memcpy(&v, (const char*)p + off, sizeof v); return v; }
template <class T> static void wr(void* p, int off, T v) { std::memcpy((char*)p + off, &v, sizeof v); }

// 1.13c D2Game+0x9EEB0 : iterate a table callback; ctx offsets: 0 count,4,0xc,0x10,0x14,0x18 arg,0x1c arg,0x20 callback
struct Hooks9EEB0 {
    uint32_t (*resolve)(uint32_t p1, uint32_t key);          // FUN 21b10
    void (*iterate)(const int32_t in[14], int32_t out[4]);   // Ordinal 10331/10591/10052 chain, out[0]=items ptr table id
    uint32_t (*item)(int32_t table, int32_t idx);            // *(table + idx*4)
    void (*cb)(uint32_t item, uint32_t a, uint32_t b);       // ctx[8]
};
bool D2GAME_9EEB0_ForEach(uint32_t p1, const uint8_t* ctx, const Hooks9EEB0& h)
{
    if (!ctx || rd<int32_t>(ctx, 0x20) == 0) return false; // retail: fatal assert + _exit
    int32_t v = rd<int32_t>(ctx, 0x14);
    if (!v) v = (int32_t)h.resolve(p1, rd<uint32_t>(ctx, 0x18));
    int32_t in[14] = {};
    in[0] = rd<int32_t>(ctx, 0); in[2] = rd<int32_t>(ctx, 4); in[3] = rd<int32_t>(ctx, 0xc);
    in[10] = rd<int32_t>(ctx, 0x10); in[11] = v;
    int32_t out[4] = {};
    h.iterate(in, out); // out[0]=count, out[1]=table
    for (int32_t i = 0; i < out[0]; ++i)
        h.cb(h.item(out[1], i), rd<uint32_t>(ctx, 0x18), rd<uint32_t>(ctx, 0x1c));
    return true;
}
// 1.13c D2Game+0x9F340 : thunk to 9EEB0 with the stack arg at +0x18
bool D2GAME_9F340_Thunk(uint32_t stackArg18, const uint8_t* ctx, const Hooks9EEB0& h)
{
    return D2GAME_9EEB0_ForEach(stackArg18, ctx, h);
}

// 1.13c D2Game+0xA2800 : fill 0x5c-byte packet, send 64 times with table entries at 0x...b770/b870
struct HooksA2800 {
    uint32_t (*getA)(uint32_t unit); // Ordinal 10867
    uint32_t (*getB)(uint32_t unit); // Ordinal 10750
    void (*send)(uint32_t p1, const uint32_t pkt[23]); // FUN 8f930
};
void D2GAME_A2800_Broadcast(uint32_t p1, uint32_t p2, uint32_t p3, const uint8_t* e, bool flag,
                            const uint32_t tblB770[64], const uint32_t tblB870[64], const HooksA2800& h)
{
    uint32_t pkt[23] = {};
    pkt[0] = 3;
    int32_t t = rd<int32_t>(e, 0);
    uint32_t sub = rd<uint32_t>(e, 0x2c);
    bool direct = (t == 2) || (t > 3 && t < 6);
    pkt[7] = direct ? rd<uint32_t>((const void*)(uintptr_t)sub, 0xc) : (sub ? h.getA(sub) : 0); // local_48 (idx 7: 0x5c-0x48.. kept by order)
    pkt[8] = direct ? rd<uint32_t>((const void*)(uintptr_t)sub, 0x10) : (sub ? h.getB(sub) : 0);
    pkt[1] = p2; pkt[3] = p3;
    if (flag) pkt[0] = 7;
    for (int i = 0; i < 64; ++i) { pkt[9] = tblB770[i]; pkt[10] = tblB870[i]; h.send(p1, pkt); }
}

// 1.13c D2Game+0xA3090 : mark unit flag, set room-owner id, then forward
struct HooksA3090 { void (*prep)(void* p1); void (*fwd)(void* p1, void* rec4, void* unit, int n); };
bool D2GAME_A3090_Apply(void* p1, uint8_t* unit, uint8_t* rec, const HooksA3090& h)
{
    if (!(rec[0] & 8) && unit) wr<uint32_t>(unit, 0xc4, rd<uint32_t>(unit, 0xc4) | 0x80000000u);
    if (rec[0] & 2) {
        h.prep(p1);
        uint32_t id = unit ? rd<uint32_t>(unit, 0xc) : 0xffffffffu;
        if (!rd<uint32_t>(rec, 4)) return false; // retail: fatal assert line 0xa33
        // inner pointer chain (rec+4)->+0x14->+0x40 is a retail 32-bit pointer: caller supplies via hook scope
        h.fwd(p1, nullptr, unit, -1); // ponytail: id store left to caller, see note
        (void)id;
    }
    int n = rd<int32_t>(rec, 0x20);
    h.fwd(p1, (void*)(uintptr_t)rd<uint32_t>(rec, 4), unit, n < 1 ? 1 : n);
    return true;
}

// 1.13c D2Game+0xA34E0 : spawn from a 0x23c-stride table row (row+0x60 = type) ; helpers injected
struct HooksA34E0 {
    uint32_t (*calc)(uint32_t unit, uint32_t type, int row, uint32_t p5); // Ordinal 10786
    uint32_t (*create)(uint32_t clientGame, int two, uint32_t off, uint32_t a, uint32_t b); // 11013
    void (*finish)(uint32_t obj, uint32_t type, int row); // 10785,10911,10807,10945,8ea80,448e0
};
uint32_t D2GAME_A34E0_Create(const uint8_t* unit, uint32_t p2, int row, uint32_t p5, const uint8_t* rows,
                             int rowCount, uint32_t game1c, uint32_t gameA8, const HooksA34E0& h)
{
    (void)p2;
    if (row < 0 || row >= rowCount) return 0;
    uint32_t type = rd<uint32_t>(rows + (size_t)row * 0x23c, 0x60);
    uint32_t off = h.calc(0, type, row, p5);
    uint32_t id = unit ? rd<uint32_t>(unit, 0xc) : 0xffffffffu, kind = unit ? rd<uint32_t>(unit, 0) : 6;
    uint32_t o = h.create(game1c, 2, gameA8 + off, kind, id);
    if (!o) return 0;
    h.finish(o, type, row);
    return o;
}

// 1.13c D2Game+0xA35D0 : null-check game then forward game+0x80
bool D2GAME_A35D0_Forward(const uint8_t* game, int p1, void (*fn)(uint32_t, int))
{
    if (!game) return false; // retail: fatal assert line 0xa2a
    fn(rd<uint32_t>(game, 0x80), p1);
    return true;
}

// 1.13c D2Game+0xA3A20 : true if another row (stride 0x3c) shares the short at +0x1e and hook(p1,idx) is set
bool D2GAME_A3A20_SharesGroup(uint32_t p1, int idx, const uint8_t* rows, int count, bool (*chk)(uint32_t, int))
{
    if (idx < 0 || idx >= count) return false;
    int16_t g = rd<int16_t>(rows + (size_t)idx * 0x3c, 0x1e);
    if (!g) return false;
    bool r = false;
    for (int i = 0; i < count; ++i)
        if (i != idx && rd<int16_t>(rows + (size_t)i * 0x3c, 0x1e) == g && chk(p1, i)) r = true;
    return r;
}

// 1.13c D2Game+0xA3AE0 : level/3 + stat/2, min 1; stat = hook(unit) if unit else 1
int D2GAME_A3AE0_Calc(int level, bool hasUnit, int (*stat)())
{
    int v = hasUnit ? stat() : 1;
    v = level / 3 + v / 2;
    return v < 2 ? 1 : v;
}

} // namespace d2game_batch_9eeb0
