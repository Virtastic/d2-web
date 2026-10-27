// Host test: g++ -std=c++17 -I<d2moo include dirs> -Wl,--unresolved-symbols=ignore-all this.cpp && ./a.out
#include "d2game_batch_2a9f0.cpp"
#include <cassert>
static int g_ret, g_n; static int32_t g_a[4];
uint16_t __stdcall COLLISION_CheckMaskWithSizeXY(D2ActiveRoomStrc*, int x, int y, unsigned w, unsigned h, unsigned short m) {
    g_n++; g_a[0] = x; g_a[1] = y; g_a[2] = w; g_a[3] = h; assert(m == 0x3f11); return g_ret;
}
using namespace d2game_wave3_1;
int main() {
    D2DrlgCoordsStrc r{}; r.nSubtileX = 10; r.nSubtileY = 20; r.nSubtileWidth = 30; r.nSubtileHeight = 30;
    D2CoordStrc p{15, 25};
    assert(placement(nullptr, r, p, 3, 4) == 1 && g_a[2] == 5 && g_a[3] == 6);
    g_ret = 1; assert(placement(nullptr, r, p, 3, 4) == 0); g_ret = 0;
    p.nX = 12; g_n = 0; assert(placement(nullptr, r, p, 3, 4) == 0 && g_n == 0);  // x <= rx+2
    p.nX = 10 + 30 - 3; assert(placement(nullptr, r, p, 3, 4) == 0);               // x >= rx-sx+w
    p.nX = 36; assert(placement(nullptr, r, p, 3, 4) == 1);
    r.nSubtileWidth = 1; p.nX = 15; assert(placement(nullptr, r, p, 3, 4) == 0);
    return 0;
}
