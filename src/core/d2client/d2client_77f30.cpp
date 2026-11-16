// 1.13c D2Client+0x77F30 (665 B, stdcall(cls), ret 4), the trade-button hover. From the disassembly:
// hit-test the mouse (B828 x, B824 y) against the button rows for `cls`, set one of three row flags, then D2Win 0xD3A8
// (ECX=1) and, on a hit, the highlight box (0xD0BA, 6 stack words) and one centred label per set flag (strings 0x1082,
// 0x1081, 0x1080: 0xD1EC ECX=id -> text, 0xD35A ECX=text -> width, 0xD37E ECX=text EDX=x, stack y,0,0).
#include <cstdint>
#include <emscripten.h>

extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_77f30(int32_t cls, int32_t mx_b828, int32_t my_b824, int32_t w_bc48, int32_t x0_b9a0,
                     int32_t y0_b9a4, int32_t y1_bc4c, uint8_t* col_bfcc,
                     uint8_t (*d390)(int32_t, int32_t, int32_t), void (*d3a8)(uint32_t ecx),
                     void (*d0ba)(int32_t, int32_t, int32_t, int32_t, int32_t, int32_t),
                     uint32_t (*d1ec)(uint32_t id), int32_t (*d35a)(uint32_t text),
                     void (*d37e)(uint32_t text, int32_t x, int32_t y, int32_t, int32_t)) {
    const int32_t x = mx_b828, y = my_b824;
    int32_t sel = 0;
    bool hit = false, f1082 = false, f1081 = false, f1080 = false;
    *col_bfcc = d390(0, 0, 0);
    const int32_t L = w_bc48 - x0_b9a0, base = y0_b9a4 + y1_bc4c;
    if (x >= L - 0x59 && x <= L + 1) {
        if (cls == 1) {
            if (y <= base - 0x9c && y >= base - 0x108) { f1081 = hit = true; sel = base - 0x108; }
            else if (y <= base - 0x108 && y >= base - 0x174) { f1080 = hit = true; sel = base - 0x174; }
        } else if (cls == 2) {
            if (y <= base - 0x108 && y >= base - 0x174) { f1080 = hit = true; sel = base - 0x174; }
            else if (y <= base - 0x30 && y >= base - 0x9c) { f1082 = hit = true; sel = base - 0x9c; }
        } else if (cls == 3) {
            if (y <= base - 0x9c && y >= base - 0x108) { f1081 = hit = true; sel = base - 0x108; }
            else if (y <= base - 0x30 && y >= base - 0x9c) { f1082 = hit = true; sel = base - 0x9c; }
        }
    }
    d3a8(1);
    if (!hit) return;
    const int32_t x0 = L - 0x59;
    d0ba(x0, sel + 5, x0 + 0x5a, sel + 5 + 0x14, (int32_t)*col_bfcc, 6);
    const bool f[3] = {f1082, f1081, f1080};
    const uint32_t id[3] = {0x1082, 0x1081, 0x1080};
    for (int i = 0; i < 3; i++) {
        if (!f[i]) continue;
        const uint32_t text = d1ec(id[i]);
        int32_t tx = w_bc48 - x0_b9a0 - 0x59;
        const int32_t room = 0x5b - tx;
        const int32_t tw = d35a(text);
        if (tw < room) tx += (room - tw) >> 1;
        d37e(text, tx, sel + 0x14, 0, 0);
    }
}
