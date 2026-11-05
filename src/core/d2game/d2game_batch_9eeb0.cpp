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

// 1.13c D2Game+0xA34E0 : EBX = game, stack (unit, s1, s2, row, p5), ret 0x14. Skill-table row (0x23c stride) spawns an object.
// Every callee is a seam in retail argument order (all stdcall; D2Common ordinals 10786, 11013, 10785, 10911, 10807, 10945).
// Fixed against the disassembly: ordinals 10785/10945 take the stack word s2, 10807/10945 take s1 (the decompile showed the row type).
struct HooksA34E0 {
    uint32_t (*calc)(uint32_t unit, uint32_t calc, int row, uint32_t p5);                         // 10786 (thunk AC9A)
    uint32_t (*create)(uint32_t clientGame, int two, uint32_t pos, uint32_t kind, uint32_t id);   // 11013 (A6A0)
    void (*initObj)(uint32_t obj, uint32_t s2);                                                   // 10785 (A5B6)
    void (*setCb)(uint32_t obj, uint32_t cbRva);                                                  // 10911 (A75A), callback = D2Game+0x9F7A0
    void (*link)(uint32_t s1, uint32_t obj, int one);                                             // 10807 (A60A)
    void (*setSkill)(uint32_t s1, uint32_t s2, int one);                                          // 10945 (A3CA)
    void (*arm)(uint32_t type, uint32_t when);                                                    // 0x8EA80 (ECX game, EDX s1)
    void (*finish)(uint32_t rowAddr, uint32_t unit, uint32_t obj, int row, uint32_t p5);          // 0x448E0 (EAX row record)
};
uint32_t D2GAME_A34E0_Create(const uint8_t* unit, uint32_t s1, uint32_t s2, int row, uint32_t p5, const uint8_t* rows,
                             int rowCount, uint32_t game1c, uint32_t gameA8, const HooksA34E0& h)
{
    if (row < 0 || row >= rowCount || !rows) return 0;
    const uint8_t* rec = rows + (size_t)row * 0x23c;
    uint32_t off = h.calc((uint32_t)(uintptr_t)unit, rd<uint32_t>(rec, 0x60), row, p5);
    uint32_t id = unit ? rd<uint32_t>(unit, 0xc) : 0xffffffffu, kind = unit ? rd<uint32_t>(unit, 0) : 6;
    uint32_t o = h.create(game1c, 2, gameA8 + off, kind, id);
    if (!o) return 0;
    h.initObj(o, s2);
    h.setCb(o, 0x9F7A0);
    h.link(s1, o, 1);
    h.setSkill(s1, s2, 1);
    h.arm(0xc, gameA8 + off);
    h.finish((uint32_t)(uintptr_t)rec, (uint32_t)(uintptr_t)unit, o, row, p5);
    return o;
}

// The A35D0, A3A20 and A3AE0 rules: d2game_a35d0.cpp, d2game_a3a20.cpp and d2game_a3ae0.cpp are their own files.

} // namespace d2game_batch_9eeb0
