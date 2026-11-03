// 1.13c D2Client+0x666D0. EAX = skip, ECX = unit, stack arg0 = EDX of 0x6CC00 (ret 4).
#include <cstring>
#include "d2client_666d0.h"
namespace d2client_666d0 {
static uint32_t rd32(const uint8_t* u, int o) { uint32_t v; memcpy(&v, u + o, 4); return v; }
static int32_t rds16(const uint8_t* u, int o) { int16_t v; memcpy(&v, u + o, 2); return v; }
// 1.13c D2Client+0x666D0
void draw_unit(Env& env, const Globals& g, uint8_t* unit, int32_t skip, uint32_t arg) {
    if (!skip && unit && (rd32(unit, 0xC8) >> 18 & 1)) return;
    if (unit && rd32(unit, 0) == 0 && rd32(unit, 0x10) == 0x11 && !env.c1fc(unit, 7)) return;
    if (!skip && unit) { uint32_t t = rd32(unit, 0); if ((int32_t)t > 0 && (int32_t)t <= 2 && (env.c1fc(unit, 0x8F) || env.c1fc(unit, 0x92))) return; }
    int32_t a = env.d07e();
    uint32_t p = env.c1ae(unit), q = env.c232(unit);
    if (a && !env.d09c(p, q)) return;
    if (!env.f66620(unit)) return;
    {
        int32_t mode = unit ? (int32_t)rd32(unit, 0) : 6, x, y;
        if (a) {
            int32_t flag = 0;
            if (unit && rd32(unit, 0) == 2) { int32_t idx = env.c226(rd32(unit, 4)); if (env.c65e(unit)[idx + 0x131] == 2) flag = 1; }
            int32_t ox, oy; env.d132(p, q, 0x60, &ox, &oy, flag);
            x = ox + (g.c9960 - g.cc418); y = oy + g.c995c - 8;
        } else { x = env.c1d2(unit); y = env.c1f6(unit); }
        auto mark = [&](int32_t r) { if (r && unit) { uint32_t f = rd32(unit, 0xC4) | 0x10000000u; memcpy(unit + 0xC4, &f, 4); } };
        if (mode == 0) { env.f728b0(unit); mark(env.cc00(unit, arg, x, y, 0, 1)); env.f72830(unit); }
        else if (mode == 1) { int32_t e = env.f4c110(unit) != 0; env.f728b0(unit); mark(env.cc00(unit, arg, x, y, e, 1)); env.f72830(unit); }
        else if (mode == 3) {
            const uint8_t* rec = env.f1580(unit ? (int32_t)rd32(unit, 4) : -1);
            if (rec) mark(env.cc00(unit, arg, x + rds16(rec, 0xA2), y + rds16(rec, 0xA4) + rds16(rec, 0xA6), 0, 1));
        } else mark(env.cc00(unit, arg, x, y, 0, 1));
    }
    if (a) env.dd138();
}
}
