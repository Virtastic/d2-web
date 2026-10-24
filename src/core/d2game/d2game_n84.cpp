// n84 batch: STALLED D2Game rows written as seam-based implementations.
// Win32/CRT/ordinal services are function-pointer seams; data is passed in.
#include <cstddef>
#include <cstdint>
#include <cstring>

namespace n84 {
using u8 = uint8_t; using u16 = uint16_t; using u32 = uint32_t; using i32 = int32_t;
template <class T> static T rd(const void* p, size_t o) { T v; memcpy(&v, (const u8*)p + o, sizeof v); return v; }
template <class T> static void wr(void* p, size_t o, T v) { memcpy((u8*)p + o, &v, sizeof v); }

// 1.13c D2GAME+0x01B3C
// rows = data-table array at dt+0xB98, count = dt+0xBA0, stride 0x23C.
int Row148Scale(const u8* rows, int count, int idx, int lvl) {
    if (idx >= 0 && idx < count) {
        const u8* r = rows + (size_t)idx * 0x23c;
        if (r && lvl > 0) return (lvl - 1) * rd<i32>(r, 0x14c) + rd<i32>(r, 0x148);
    }
    return 0;
}

// 1.13c D2GAME+0x0679B
struct HeapTermSeams {
    int (*VirtualFree)(void*, size_t, u32); int (*HeapFree)(void*, u32, void*); int (*HeapDestroy)(void*);
};
struct HeapTermState { int mode; int count; u8* regions; void* heap; };
void HeapTerm(const HeapTermSeams& s, HeapTermState& g) {
    if (g.mode == 3) {
        u8* r = g.regions;
        for (int i = 0; i < g.count; ++i, r += 0x14) {
            void* base = rd<void*>(r, 0xc);
            s.VirtualFree(base, 0x100000, 0x4000);
            s.VirtualFree(base, 0, 0x8000);
            s.HeapFree(g.heap, 0, rd<void*>(r, 0x10));
        }
        s.HeapFree(g.heap, 0, g.regions);
    }
    s.HeapDestroy(g.heap);
}

// 1.13c D2GAME+0x077F0
void GlobalUnwind2(void (*RtlUnwind)(void*, void*, void*, void*), void* frame, void* target) {
    RtlUnwind(frame, target, nullptr, nullptr);
}

// 1.13c D2GAME+0x07C65
struct SetMbcpSeams {
    void (*lock)(int); void (*unlock)(); u32 (*GetOEMCP)(); u32 (*GetACP)();
    void* (*malloc)(size_t); void (*free)(void*); int (*setCP)(u32);  // setCP = FUN 0x7AD1
};
struct MbcpState {
    int userDefault; u32 cpUser /*6fd3004c*/; u32 cp /*a0e4*/; int* cache /*9fd4*/;
    int g_d8, g_d0; u16 g_f0[5]; u8 g_e0[0x101]; u8 g_100[0x100];
};
int SetMbcp(const SetMbcpSeams& s, MbcpState& g, int cpArg) {
    int ret = -1;
    s.lock(0xd);
    g.userDefault = 0;
    if (cpArg == -2) { g.userDefault = 1; cpArg = (int)s.GetOEMCP(); }
    else if (cpArg == -3) { g.userDefault = 1; cpArg = (int)s.GetACP(); }
    else if (cpArg == -4) { g.userDefault = 1; cpArg = (int)g.cpUser; }
    if ((u32)cpArg == g.cp) ret = 0;
    else {
        int* m = g.cache;
        if (!m || *m != 0) m = (int*)s.malloc(0x220);
        if (m && (ret = s.setCP((u32)cpArg)) == 0) {
            u8* b = (u8*)m;
            wr<i32>(b, 0, 0); wr<u32>(b, 4, g.cp); wr<i32>(b, 8, g.g_d8); wr<i32>(b, 12, g.g_d0);
            for (int i = 0; i < 5; ++i) wr<u16>(b, 0x10 + i * 2, g.g_f0[i]);
            for (int i = 0; i < 0x101; ++i) b[0x1c + i] = g.g_e0[i];
            g.cache = m;
            for (int i = 0; i < 0x100; ++i) b[0x11d + i] = g.g_100[i];
        }
        if (ret == -1 && m != g.cache) s.free(m);
    }
    s.unlock();
    return ret;
}

// 1.13c D2GAME+0x09A45
struct MemInfo { u8* base; u8* allocBase; size_t region; u32 state, protect; };
struct ResetStkSeams {
    size_t (*VirtualQuery)(const void*, MemInfo*); u32 (*PageSize)();
    void* (*VirtualAlloc)(void*, size_t, u32, u32); int (*VirtualProtect)(void*, size_t, u32, u32*);
};
int ResetStkOflw(const ResetStkSeams& s, int osMode, const void* sp) {
    MemInfo mi;
    if (!s.VirtualQuery(sp, &mi)) return 0;
    uintptr_t pg = s.PageSize();
    u8* cur = (u8*)((~(pg - 1) & (uintptr_t)sp) - pg);
    u8* lim = mi.allocBase + ((osMode != 1 ? 0x1 : 0x11) * pg);
    if (lim > cur) return 0;
    u8* t = cur;
    if (osMode != 1) {
        t = mi.allocBase;
        do { if (!s.VirtualQuery(t, &mi)) return 0; t += mi.region; } while (!(mi.state & 0x1000));
        t = mi.base;
        if (mi.protect & 0x100) return 1;
        if (cur < mi.base) return 0;
        if (mi.base < lim) t = lim;
        s.VirtualAlloc(t, pg, 0x1000, 4);
    }
    u32 old;
    return s.VirtualProtect(t, pg, osMode != 1 ? 0x104 : 1, &old);
}

// 1.13c D2GAME+0x0EE90
// Unit-fatigue style tier chooser. ord: 10718 isPlayer,10744 stat,10560 record,10695 ?,10411 ?,
// 211d0 = bitset-test. Order table is DAT_6fd1d09c (6 dwords).
struct EE90Seams {
    short (*isPlayerLike)(void*); int (*getStat)(void*, int); void* (*getRec)(u32, u8);
    void* (*getCtx)(u32); void* (*getBits)(void*); u32 (*bitsBusy)(void*);
    void (*fatal)(int);
};
u32 TierPick(const EE90Seams& s, u8* unit, const i32 order[6], const u8* levelTbl) {
    short pl = s.isPlayerLike(unit);
    if (pl && rd<u32>(unit, 0x30)) return rd<u32>(unit, 0x30);
    if (!unit) { s.fatal(0xa2a); return 0; }
    u8* pd = rd<u8*>(unit, 0x80);
    if (!pd) { s.fatal(0x482); return 0; }
    int bias = (pl == 0 ? 0 : -100) + 100;  // (-(sVar==0)&-100)+100
    bias = (-(int)(pl == 0) & -100) + 100;
    i32* rec = (i32*)s.getRec(rd<u32>(unit, 4), pd[0x6d]);
    if (!rec) { s.fatal(0x486); return 0; }
    u8* ctx = (u8*)s.getCtx(rd<u32>(unit, 4));
    if (!ctx) { s.fatal(0x489); return 0; }
    if (ctx[0x12a]) return 2;
    int hp = rd<i32>(unit, 0xc), v = 0;
    if (pl && (s.getStat(unit, 0x34) != 0 || (hp -= levelTbl[bias + 0xfd]) < 1)) hp = 1;
    u32 tier = 1;
    for (u32 i = 0; i < 6; ++i) {
        int k = order[i];
        if (!pl) {
            switch (k) {
            case 2: v = rec[0xe] - hp / rec[0xf]; tier = 2; break;
            case 3: v = rec[0xc] - hp / rec[0xd]; tier = 3; break;
            case 4: v = rec[9] - hp / rec[10]; tier = 4; break;
            case 5: v = rec[6] - hp; tier = 5; break;
            case 6: v = rec[3] - hp; tier = 6; break;
            case 7: v = rec[0] - hp; tier = 7; break;
            }
        } else {
            switch (k) {
            case 2: v = rec[0xe] - hp / rec[0xf]; tier = 2; break;
            case 3: v = rec[0xc] - hp / rec[0xd]; tier = 3; break;
            case 4: v = rec[9] - hp / rec[10]; tier = 4; break;
            case 5: v = rec[6] - hp / rec[7]; tier = 5; break;
            case 6: v = rec[3] - hp / rec[4]; tier = 6; break;
            case 7: v = rec[0] - hp / rec[1]; tier = 7; break;
            default: break;
            }
            if (v < 1) return tier;
        }
        if (v < 1) v = 1;
        if (!pl || true) { if (!s.bitsBusy(s.getBits(ctx))) return tier; }
    }
    return (rd<u8>(unit, 0x80) & 0x40 | 0x20) >> 5;
}

// 1.13c D2GAME+0x1F600
// Compresses a monster unit into a 0x5C-byte inactive record and links it into the per-type list (callee of 0x204E0).
struct F600Seams {
    void* (*game)(); void* (*allocRec)(); int (*dtRowCount)(); int (*rowActive)(int); int (*rowCond)(int);
    void (*rowA)(int); void (*rowB)(int); void (*removeFromRoom)(void*);
    u32 (*unitX)(void*); u32 (*unitY)(void*); int (*mode)(void*);
    u32 (*getUnitType)(void*); void (*fillInfo)(void*, void*); void (*removeEvent)(void*);
    u32 (*coordA)(void*, int); u32 (*coordB)(void*); u32 (*coordC)(void*, int);
    void (*updateRoom)(void*, void*); int (*aiKind)(void*); void (*nihRemove)(void*);
    void* (*listFor)(int, u32, int, int); void (*fatal)(int);
};
void RemoveNihlathakFromTown(const F600Seams& s, u8* unit, u8* gameArg) {
    void* g = s.game();
    if (!unit || !g) return;
    for (int i = 0, n = s.dtRowCount(); i < n; i++)
        if (s.rowActive(i) && s.rowCond(i)) { s.rowA(i); s.rowB(i); }
    for (u8* ev = rd<u8*>(unit, 0xdc); ev;) {
        u8 t = *ev; ev = rd<u8*>(ev, 0x24);
        if (t == 2) s.removeEvent(unit);
    }
    u32* rec = (u32*)s.allocRec();
    memset(rec, 0, 0x5c);
    rec[7] = 0xffffffff;
    int ty = rd<i32>(unit, 0);
    bool mon = ty == 1;
    u8* d = rd<u8*>(unit, 0x14);
    rec[0] = s.unitX(unit); rec[1] = s.unitY(unit);
    rec[2] = rd<u32>(unit, 4);
    rec[0x15] = rd<u32>(gameArg, 0xa8);
    rec[10] = (mon && d) ? rd<u32>(rd<u8*>(d, 0x28), 0) : 0;
    wr<u16>(rec, 0x30, (mon && d) ? rd<u16>(d, 0x14) : 0x1506);
    s.fillInfo(gameArg, rec);
    if (mon && d && (d[0x16] & 4)) rec[6] |= 2;
    if (mon) {
        if (d && (d[0x16] & 2)) { rec[6] |= 0x800; wr<u16>(rec, 0x3c, d ? rd<u16>(d, 0x26) : 0xffff); }
        if (d && (d[0x16] & 1)) {
            rec[6] |= 1;
            u8* ai = rd<u8*>(unit, 0x74);
            if (ai) {
                u8* a0 = rd<u8*>(ai, 0);
                if (s.aiKind(unit) == 2 && rd<i32>(a0, 0xe0) == 0x14 && a0[10] == 1) s.nihRemove(unit);
            }
        }
    }
    if (mon && d && (d[0x16] & 0x10)) rec[6] |= 0x20;
    if (mon && d && (d[0x5c] & 2)) rec[6] |= 0x40;
    int m = s.mode(unit);
    if (m == 2) rec[6] |= 0x100; else if (m == 0) rec[6] |= 0x200; else if (m == 1) rec[6] |= 0x80;
    if (rd<i32>(unit, 0xd0) <= 7) { s.fatal(0); return; }
    if (rd<i32>(unit, 0xd0) != 0xb) rec[6] |= 0x400;
    rec[0x10] = s.coordA(unit, 0xd); rec[0x11] = s.coordB(unit); rec[0x12] = s.coordC(unit, 0xd);
    rec[0xb] = (mon && d) ? rd<u32>(d, 0x58) : 0;
    if (mon && d) memcpy((u8*)rec + 0x32, d + 0x1c, 9);  // bytes 0x32..0x3a from d+0x1c..0x24
    u32 ds = rd<u32>(unit, 0x10);
    if (ds == 0xc || ds == 0) rec[6] |= 4;
    if (mon) {
        u8* p = d ? rd<u8*>(d, 0x28) : nullptr;
        if (rd<i32>(p, 0x30) == 1) {
            u16 fl = rd<u16>(p, 8); u32 a = rd<u32>(p, 0x2c), b = rd<u32>(p, 0x34);
            wr<u32>(p, 0x2c, 0xffffffff); wr<u32>(p, 0x34, 0);
            if (a != 0xffffffff) { rec[7] = a; rec[8] = b; if (fl & 1) rec[6] |= 8; if (fl & 2) rec[6] |= 0x10; }
        }
    }
    rec[3] = rd<u32>(unit, 0xc); rec[4] = rd<u32>(unit, 0xc4); rec[5] = rd<u32>(unit, 0xc8);
    s.updateRoom(unit, rec);
    u32* list = (u32*)s.listFor(0, s.getUnitType(unit) & 0xff, 0, 1);
    rec[0x16] = list[3];
    list[3] = (u32)(uintptr_t)rec;  // ponytail: ptr truncated as in retail 32-bit layout
}

// 1.13c D2GAME+0x26230
struct CorpseCloudSeams {
    int (*stat)(void*, int, int); int (*unitY)(void*); int (*unitX)(void*);
    void (*cast)(void*, int, void*, int, int, int, int, int, int, int);
};
void CastCorpsePoisonCloud(const CorpseCloudSeams& s, void* game, void* unit) {
    int lvl = s.stat(unit, 0xc, 0);
    if (lvl < 2) lvl = 1;
    int y = s.unitY(unit), x = s.unitX(unit);
    s.cast(game, 0x9b, unit, 0, lvl, 0, 0, x, y, 1);
}

// 1.13c D2GAME+0x29AE0
void GetMemoryUsage(u32 (*ord10011)(int), u32 gLast, u32* out0, u32* out1) {
    *out0 = ord10011(0);
    *out1 = gLast;
}

// 1.13c D2GAME+0x29BA0
void UnlockGame(u8* game, void (*Leave)(void*), void (*fatal)(int)) {
    if (!game) { fatal(0x100e); return; }
    void* cs = rd<void*>(game, 0x18);
    if (!cs) { fatal(0x100f); return; }
    Leave(cs);
}

// 1.13c D2GAME+0x2A330
void SetServerCallbackFunctions(void* cb, void** gCb, int* gEnabled, void (*ord10352)()) {
    if (cb) { *gCb = cb; *gEnabled = 1; ord10352(); }
    else { *gCb = nullptr; *gEnabled = 0; }
}

// 1.13c D2GAME+0x2AAE0
struct GameLookupSeams {
    void (*Enter)(void*); void (*Leave)(void*); u8* (*find)(u8* tbl);
    void (*touch)(); void (*fatal)(int);
};
u8* GameLookup(const GameLookupSeams& s, u8* table) {
    if (!table) return nullptr;
    void* cs = table + 0x50;
    s.Enter(cs);
    u8* g = s.find(table);
    if (!g) { s.Leave(cs); return nullptr; }
    if (rd<u32>(g, 0x18) == 0) { s.fatal(0x112e); return nullptr; }
    s.touch();
    s.Leave(cs);
    return g;
}
}  // namespace n84
