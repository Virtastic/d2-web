// D2Client+0x89830: raise the "Aux Battle.net" registry IP's last octet to the tier of table[eax]; env-seam style.
#include <cstdint>
#include <cstdio>
#include <emscripten.h>
struct D2c89830Env {
    uint32_t tableVal;                                      // dword at D2Client+0x91310 + eax*12
    int32_t (*regRead)(const char* app, const char* key, int32_t z, char* buf, int32_t cap);  // Ordinal_422
    void (*regWrite)(const char* app, const char* key, int32_t z, const char* val);          // Ordinal_425
    uint32_t keyAddrLow;                                    // value of the "Aux Battle.net" pointer, kept when sscanf stores nothing
};
extern "C" {
// 1.13c D2Client+0x89830
EMSCRIPTEN_KEEPALIVE void d2c_89830(const D2c89830Env* e) {
    char buf[100];
    uint32_t last = e->keyAddrLow;
    uint8_t cur;
    if (!e->regRead("Diablo II", "Aux Battle.net", 0, buf, 100)) cur = 0x22;
    else {
        int a, b, c, d;
        if (sscanf(buf, "%d.%d.%d.%d", &a, &b, &c, &d) == 4) last = (uint32_t)d;
        if (last & 1) cur = 7;
        else if ((int8_t)last < 0) cur = 6;
        else if (last & 0x10) cur = 5;
        else if (last & 8) cur = 4;
        else if (last & 0x40) cur = 3;
        else cur = ((last & 4) != 0) + 1;
    }
    uint8_t want;
    switch (e->tableVal) {
    case 2: want = 2; break; case 3: want = 3; break; case 4: want = 4;  break;
    case 5: case 6: want = 5; break; case 7: want = 7; break; default: want = 1;
    }
    if (cur < want) {
        int o = want == 2 ? 0x26 : want == 3 ? 0x62 : want == 4 ? 0x2a : want == 5 ? 0xb2 : want == 7 ? 0x23 : 0x22;
        snprintf(buf, sizeof buf, "%d.%d.%d.%d", 0xd8, 0x94, 0xf6, o);
        e->regWrite("Diablo II", "Aux Battle.net", 0, buf);
    }
}
}
