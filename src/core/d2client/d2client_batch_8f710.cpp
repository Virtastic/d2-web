// src/core/d2client/d2client_batch_8f710.cpp -- 1.13c D2Client.dll sub_8F710 (alpha lane). Base 0x6FAB0000.
// Unrecovered callees are host seams; globals are passed in a state struct (32-bit retail layout).
#include <cstdint>
#include <cstddef>
#include <emscripten.h>

struct D2ClientSub8F710Tab {   // 0x12-byte records at D2Client+0xDC182 (DAT_6fb91182)
    uint32_t selected;         // +0
    uint32_t populated;        // +4 (DAT_6fb91186)
    uint8_t  rest[10];
} __attribute__((packed));
static_assert(sizeof(D2ClientSub8F710Tab) == 0x12, "tab stride");
static_assert(offsetof(D2ClientSub8F710Tab, populated) == 4, "populated");

struct D2ClientSub8F710State {
    uint32_t busy;             // DAT_6fbcbc4c; nonzero -> return
    uint8_t  playerValid;      // DAT_6fbc9721
    uint8_t  curTab;           // DAT_6fbcbc48 (out)
    uint8_t  maxTab;           // DAT_6fbcbc50
    uint32_t tabCount;         // DAT_6fb911c0
    D2ClientSub8F710Tab* tabs; // DAT_6fb91182
};

struct D2ClientSub8F710Imports {
    // player unit by DAT_6fbc971d: 0 = not found; *type out (unit+0 must be 1)
    uint32_t (*findPlayer)(uint32_t* type);
    uint32_t (*invFirst)(uint32_t player);    // unit+0x60 -> D2Common 10460 (0 when no inventory)
    uint32_t (*invNext)(uint32_t item);       // D2Common 10464
    uint8_t  (*itemTab)(uint32_t item);       // D2Common 11147 then 10020
    void     (*halt)(void);                   // type != 1: Fog halt, _exit(-1)
};

// 1.13c D2Client+0x8F710
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_fn_d2client_8f710(uint32_t tab, D2ClientSub8F710State* s, const D2ClientSub8F710Imports* im) {
    uint8_t t = (uint8_t)tab;
    if (t >= 5) t = 0;
    if (s->busy || !s->playerValid) return;
    uint32_t type = 0;
    uint32_t player = im->findPlayer(&type);
    if (!player) return;
    if (type != 1) { im->halt(); return; }
    uint32_t it = im->invFirst(player);
    if (!it) return;
    uint32_t cnt[5] = {0, 0, 0, 0, 0};
    for (; it; it = im->invNext(it)) {
        uint8_t k = im->itemTab(it);
        if (k < 5) cnt[k]++;
    }
    s->curTab = t;
    for (uint32_t i = 0; (int32_t)i < (int32_t)s->tabCount; i++) {
        s->tabs[i].selected = 0;
        s->tabs[i].populated = cnt[i] != 0;
    }
    if (cnt[t] == 0) {
        int n = 0;
        do {
            t++; n++;
            if (t > s->maxTab || t == 0xff) t = 0;
        } while (cnt[t] == 0 && n < 4);
        s->curTab = t;
    }
    s->tabs[t].selected = 1;
}
