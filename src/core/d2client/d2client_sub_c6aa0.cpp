// 1.13c D2Client+0xC6AA0 -- derived from the decompile. Env-seam; ordinal names UNVERIFIED.
#include <cstdint>
struct ClientC6AA0Env {
    int32_t (*rd)(uint32_t addr);                                      // 32-bit read of guest memory
    void (*wr)(uint32_t addr, int32_t v);
    int32_t (*call)(uint32_t va, int32_t a, int32_t b, int32_t c, int32_t d, int32_t e); // D2Client callee by Ghidra VA
    int32_t (*ord)(uint32_t ordinal, int32_t a);
    uint32_t dataTables;   // *sgptDataTables_exref
};
// unit = EAX, bx = EBX (compared to target x), retFlag = caller-stack slot read as unaff_retaddr, p1 = stack arg.
extern "C" int32_t d2_client_c6aa0(const ClientC6AA0Env& e, uint32_t unit, int32_t bx, int32_t retFlag, int32_t p1) {
    e.ord(10980, p1);
    int32_t y = e.ord(10649, p1);
    uint32_t pth = 0, ex = 0;
    int32_t room = 0;
    if (unit && e.rd(unit + 0x54) && (ex = e.rd(e.rd(unit + 0x54) + 0x30)) != 0) room = e.rd(ex + 0xc) >> 11;
    int32_t type = unit ? e.rd(unit) : 6;
    bool notPlayer = type != 1;
    int32_t cls = -1;
    if (!notPlayer && unit && e.rd(unit + 4) >= 0) {
        cls = e.rd(unit + 4);
        if (cls < e.rd(e.dataTables + 0xa80)) {
            uint32_t rec = cls * 0x1a8 + e.rd(e.dataTables + 0xa78);
            if (rec) cls = (int16_t)(e.rd(rec) >> 16);  // short at +2
        }
        if (cls < 0 || e.rd(e.dataTables + 0xa80) <= cls) cls = -1;
    }
    (void)pth;
    bool same = false;
    if (room != 0) {
        auto fld = [&](uint32_t off, uint32_t ordn) {
            int32_t t = e.rd(unit);
            uint32_t p = e.rd(unit + 0x2c);
            if (t == 2 || (3 < t && t < 6)) return e.rd(p + off);
            return p ? e.ord(ordn, p) : 0;
        };
        same = fld(0xc, 10867) == bx && fld(0x10, 10750) == y;
    }
    if (same || room == 0) {
        e.call(0x6fb76060, 0, 0, 0, 0, 0);
        if (unit && e.rd(unit + 0x54) && (ex = e.rd(e.rd(unit + 0x54) + 0x30)) != 0) {
            e.wr(ex + 4, 0); e.wr(ex + 8, 0); e.wr(ex + 0xc, 0);
        }
        if (retFlag) e.call(0x6fb1b300, unit, 2, 0, 0, 0);
        e.call(0x6fad2fb0, 1, 0, 0, 0, 0);
        e.call(0x6fb38a70, unit, 0, 0, 0, 0);
        if (notPlayer) e.call(0x6fb38a70, unit, 0, 0, 0, 0);
        else if (cls == 0x4e) e.call(0x6fb76150, unit, 0, 0, 0, 0);
        return 1;
    }
    if (!notPlayer && cls != 0x4e && cls == 0x21c) e.ord(10080, 0);
    e.ord(10542, 0);
    e.wr(unit + 0x48, (e.rd(unit + 0x48) & 0xffffff00u) + 0x100);
    return 0;
}
