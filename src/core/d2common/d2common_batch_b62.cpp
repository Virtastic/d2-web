// 1.13c D2Common batch b62 (register-arg rows). Hidden register inputs (in_EAX/unaff_ESI/EDI/EBX)
// become explicit leading parameters, named per row. Bodies read the retail 32-bit struct layout
// by byte offset (rd/wr below); pointer fields are 32-bit retail values. Callees are the retail
// sub_ names, declared weak so the file links before they are named.
// Skipped: +0x0DB70 (FUN_6fd8e310 returns two shorts through an unpinned register/stack pair).
#include <cstdint>
#include <cstring>

#define W extern "C" __attribute__((weak))
W void* sub_87F80(void* statList, int32_t id);
W void* sub_88100(void* a, int32_t b);
W void  sub_8A7D0(void* list, void* stat, int32_t flag);
W void  D2Common_ExpireStatList_6FDB6E30(void*);
W void  D2Common_STATLIST_FreeStatListImpl_6FDB7050(void*);
W void  sub_8E080(int32_t);
W uint8_t sub_7EDA0(void*);
W int32_t sub_85530();
W void  sub_B6AA0(void*);
W void  sub_B6920(void*);
W void* sub_7D9B0(void*, int32_t);
W void* sub_7E5C0(int32_t, int32_t, void*, void*);
W void  sub_CC090();
W void  sub_B8400();
W void  sub_5BF70(void*);
W void  sub_CC0E0();
W int32_t sub_5D0A0(int32_t, void*, int32_t* out2);
W int32_t sub_5D580(int32_t, int32_t, int32_t*, uint32_t*);
W int32_t sub_5D820(void*, int32_t);
#undef W

template <class T> static T rd(const void* p, size_t o) { T v; std::memcpy(&v, (const char*)p + o, sizeof v); return v; }
template <class T> static void wr(void* p, size_t o, T v) { std::memcpy((char*)p + o, &v, sizeof v); }
static void* rdp(const void* p, size_t o) { return (void*)(uintptr_t)rd<uint32_t>(p, o); }
[[noreturn]] static void fatal() { __builtin_trap(); } // retail: Ordinal_10024(msg); _exit(-1)

// 1.13c D2Common+0x522C0: register map EDI = pUnit (this unit), EBX = stat-list id; stack param_1 = bCreate.
void* sub_522C0(void* pUnitEDI, int32_t idEBX, int32_t bCreate)
{
    void* l = rdp(pUnitEDI, 0x5c);
    void* r = nullptr;
    if (l && rd<int32_t>(l, 0x10) < 0) r = sub_87F80(l, idEBX);
    if (bCreate) {
        if (!r) {
            r = sub_88100(rdp(pUnitEDI, 8), 0);
            if (r) wr<int32_t>(r, 0x14, idEBX);
            sub_8A7D0(pUnitEDI, r, 1);
        }
        return r;
    }
    if (r) {
        D2Common_ExpireStatList_6FDB6E30(r);
        if (rd<int32_t>(r, 0x10) >= 0) D2Common_STATLIST_FreeStatListImpl_6FDB7050(r);
    }
    return nullptr;
}

// 1.13c D2Common+0x35470: register map EDI = pCtx (+0x30 -> pathInfo), ESI only feeds a fatal-code arg.
int32_t sub_35470(void* pCtxEDI)
{
    char* p1 = (char*)rdp(pCtxEDI, 0x30);
    char* u = (char*)rdp(p1, 0x58);
    if (!u) return 1;
    auto dim = [&](size_t a, size_t b) -> int16_t {
        int32_t t = rd<int32_t>(u, 0);
        char* e = (char*)rdp(u, 0x2c);
        if (t == 2 || (t > 3 && t < 6)) return (int16_t)rd<int32_t>(e, a);
        return e ? rd<int16_t>(e, b) : 0;
    };
    int16_t sx = dim(0xc, 2);  wr<int16_t>(p1, 0x10, sx);
    int16_t sy = dim(0x10, 6); wr<int16_t>(p1, 0x12, sy);
    if (!sx && !sy) return 0;
    int32_t t = rd<int32_t>(u, 0);
    if (t == 0 || t == 1) {
        if (rd<char>(p1, 0x68)) sub_8E080(t);
        return 2;                       // retail falls into case 4 when +0x68 != 0
    }
    if (t == 2) {
        if (!sub_7EDA0(u)) return 0;
        p1 = (char*)rdp(pCtxEDI, 0x30);
        u = (char*)rdp(p1, 0x58);
        if (!u) fatal();
        char* cls = (char*)rdp(u, 0x14);
        if (rd<int32_t>(u, 0) != 2 || !rd<char>(rdp(cls, 0), 0x13a)) fatal();
        char* e = (char*)rdp(u, 0x2c);
        if (rd<char>(rdp(cls, 0), 0x13c) != 1) {
            wr<int16_t>(p1, 0x10, rd<uint16_t>(p1, 2) < rd<uint32_t>(e, 0xc) ? rd<int16_t>(p1, 0x10) - 2 : rd<int16_t>(p1, 0x10) + 2);
            return 2;
        }
        wr<int16_t>(p1, 0x12, rd<uint16_t>(p1, 6) < rd<uint32_t>(e, 0x10) ? rd<int16_t>(p1, 0x12) - 2 : rd<int16_t>(p1, 0x12) + 2);
        return 2;
    }
    if (t == 4) return 2;
    return 1;
}

// 1.13c D2Common+0x35780: register map EDI = pCtx.
int32_t sub_35780(void* pCtxEDI)
{
    int32_t k = rd<int32_t>(pCtxEDI, 0x3c);
    uint32_t f;
    if (k == 4) {
        int32_t r = sub_85530();
        f = rd<uint32_t>(pCtxEDI, 0x34);
        if (!r) { wr<uint32_t>(pCtxEDI, 0x34, f & 0xffffffdf); return 0; }
    } else {
        if (k == 10) {
            sub_B6AA0(pCtxEDI);
            wr<uint32_t>(pCtxEDI, 0x34, rd<uint32_t>(pCtxEDI, 0x34) | 0x20);
            return rd<int32_t>(pCtxEDI, 0x28);
        }
        if (k != 0xe) fatal();
        sub_B6920(pCtxEDI);
        f = rd<uint32_t>(pCtxEDI, 0x34);
    }
    wr<uint32_t>(pCtxEDI, 0x34, f | 0x20);
    return rd<int32_t>(pCtxEDI, 0x28);
}

// 1.13c D2Common+0x7C300: __thiscall this = key (compared with +0x1d0), ESI = pOwner (list head at +0x47c, next at +0x1ac).
void sub_7C300(void* key, int32_t p1, int32_t p2, void* pOwnerESI)
{
    void* n = rdp(pOwnerESI, 0x47c);
    for (; n; n = rdp(n, 0x1ac))
        if (rdp(n, 0x1d0) == key) break;
    if (!n) n = sub_7D9B0(pOwnerESI, (int32_t)(intptr_t)key);
    void* r = sub_7E5C0(p1, p2, pOwnerESI, n);
    if (rd<int16_t>(r, 0xe) == 0) sub_CC090();
}

// 1.13c D2Common+0x7C270: register map EAX = pUnit.
uint32_t sub_7C270(void* pUnitEAX)
{
    uint32_t f = rd<uint32_t>(pUnitEAX, 0x28);
    if (!(f & 0x1000000)) sub_B8400();
    if (!(rd<uint32_t>(pUnitEAX, 0x28) & 0x2000000) && rd<int32_t>(pUnitEAX, 0x48) == 2) sub_5BF70(pUnitEAX);
    if (!(rd<uint32_t>(pUnitEAX, 0x28) & 0x100000)) sub_CC0E0();
    return rd<uint32_t>(pUnitEAX, 0x30);
}

// 1.13c D2Common+0x0D8E0: register map EAX = pPath; stack param_1 = int32_t out[2], param_2 = uint32_t* out.
bool sub_D8E0(void* pPathEAX, int32_t* out, uint32_t* pOut)
{
    char* p = (char*)pPathEAX;
    void* t = rdp(p, 0x30);
    wr<int16_t>(p, 0x54, 0);
    *pOut = 0;
    bool b = t && rd<int32_t>(t, 0) == 1 && (rd<uint32_t>(p, 0x34) & 0x10);
    if (rd<int32_t>(p, 0x72) == 0 && rd<int32_t>(p, 0x76) == 0) {
        wr<uint32_t>(p, 0x28, 0); wr<uint32_t>(p, 0x24, 0);
        out[0] = (int32_t)((rd<uint32_t>(p, 0) & 0xffff0000) + 0x8000);
        out[1] = (int32_t)((rd<uint32_t>(p, 4) & 0xffff0000) + 0x8000);
        return false;
    }
    int32_t d[2];
    int32_t r4 = sub_5D0A0(0, p, d);
    int32_t x = d[0] + rd<int32_t>(p, 0), y = d[1] + rd<int32_t>(p, 4);
    if ((int16_t)(rd<uint32_t>(p, 0) >> 16) != (int16_t)((uint32_t)x >> 16) ||
        (int16_t)(rd<uint32_t>(p, 4) >> 16) != (int16_t)((uint32_t)y >> 16)) {
        int32_t s = rd<int32_t>(p, 0x3c);
        if (rd<char>(p, 0x90) && s != 8 && s != 0xb) wr<char>(p, 0x90, rd<char>(p, 0x90) - 1);
        if (sub_5D580(d[0], d[1], out, pOut)) {
            if (!b) { wr<uint32_t>(p, 0x24, rd<uint32_t>(p, 0x28)); return false; }
            return !sub_5D820(t, rd<int32_t>(p, 0x58) != 0);
        }
    }
    out[0] = x; out[1] = y;
    if (!r4) return false;
    wr<uint32_t>(p, 0x24, rd<uint32_t>(p, 0x24) + 1);
    return true;
}
