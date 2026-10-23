// 1.13c D2Client+0x25DC0 / +0x25F10: fire six missile-create requests (0xB30F0) around a unit; differ only in req[4] (0x30 / 0x2d).
#include <cstdint>
// req = 23 dwords: [0]=0x520 [1]=p2 [2]=unit [4]=kind [5]=x [6]=y [7]=x+dx [8]=y+dy [9]=1 [0xc]=clamp(ord10973(p2,0xc,0)/5,1,8).
// Unit pos: type 2 or 4..5 -> *(u32*)(u[0xb]+0xc/+0x10); else u[0xb]==0 ? 0 : ord10867/10750(u[0xb]).
template <class Rd, class O1, class O2, class O3, class Create>
static void d2client_fan25(const int32_t* u, uint32_t p2, uint32_t kind, Rd rd, O1 o10867, O2 o10750, O3 o10973, Create create) {
    static const int32_t dy[6] = {6, -6, 6, -6, 6, -6}, dx[6] = {-6, -6, 0, 0, 6, 6};
    for (int k = 0; k < 6; k++) {
        int32_t t = u[0], s = u[0xb];
        bool sp = t == 2 || (t > 3 && t < 6);
        int32_t x = sp ? (int32_t)rd(s + 0xc) : s == 0 ? 0 : (int32_t)o10867(s);
        int32_t y = sp ? (int32_t)rd(s + 0x10) : s == 0 ? 0 : (int32_t)o10750(s);
        uint32_t r[23] = {0};
        r[0] = 0x520; r[1] = p2; r[2] = (uint32_t)(uintptr_t)u; r[4] = kind;
        r[5] = x; r[6] = y; r[7] = x + dx[k]; r[8] = y + dy[k]; r[9] = 1;
        int32_t l = (int32_t)o10973(p2, 0xc, 0) / 5;
        r[0xc] = l < 1 ? 1 : l > 8 ? 8 : l;
        create(r);
    }
}
template <class Rd, class O1, class O2, class O3, class C>
void D2Client_Sub25DC0(const int32_t* u, uint32_t p2, Rd a, O1 b, O2 c, O3 d, C e) { d2client_fan25(u, p2, 0x30u, a, b, c, d, e); }
template <class Rd, class O1, class O2, class O3, class C>
void D2Client_Sub25F10(const int32_t* u, uint32_t p2, Rd a, O1 b, O2 c, O3 d, C e) { d2client_fan25(u, p2, 0x2du, a, b, c, d, e); }
