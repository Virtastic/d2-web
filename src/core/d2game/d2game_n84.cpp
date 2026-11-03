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

// 1.13c D2GAME+0x1F600 moved to src/core/d2game/d2game_1f600.cpp

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
