// 1.13c D2Client+0x6F4A0 (260 B): IME/input shutdown. Imports and globals are host seams.
#include <cstdint>
#include <emscripten.h>

struct D2ClientImeClose6f4a0Env {
    int32_t (*ord10048)();                       // window handle
    void (*fatal)(uint32_t strId);               // Ordinal_10265(0x18c) -> Ordinal_10024 -> _exit(-1)
    void (*freeEntry)(int32_t);                  // FUN_6faeca00
    void (*log)(int32_t hwnd, uint32_t strRva);  // FUN_6faecc60, string 0x6FB92808
    int32_t (*immOpenOff)(int32_t hwnd);         // ImmGetContext + ImmSetOpenStatus(0); returns 0 if no context
    void (*immConvOff)(int32_t hwnd);            // ImmGetConversionStatus/SetConversionStatus(0,..); releases context
    void (*ord10043)(uint32_t a, uint32_t b);
    const int32_t* tbl;                          // 0x6FB927DC, stride 0xC, 4 entries (as dwords, stride 3)
    uint8_t imeOn;                               // DAT_6fbcc038
    uint32_t gameType;                           // DAT_6fbc3f60
    uint32_t comp, read;                         // _DAT_6fbcec40, _DAT_6fbcfe98
    uint32_t flag34;                             // DAT_6fbcc034
};

// 1.13c D2Client+0x6F4A0
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_6f4a0(D2ClientImeClose6f4a0Env* e) {
    if (!e->ord10048()) { e->fatal(0x18c); return; }
    for (uint32_t i = 0; i < 4; i++) e->freeEntry(e->tbl[i * 3]);
    if (!e->imeOn) return;
    e->log(e->ord10048(), 0x6FB92808);
    if (e->immOpenOff(e->ord10048())) {
        // convert-status reset only for game type 7 or 0xc; host seam applies it, gated here
        if (e->gameType == 7 || e->gameType == 0xc) e->immConvOff(e->ord10048());
        else e->immConvOff(0);   // release only (hwnd 0 = no conversion reset)
    }
    e->comp = 0; e->read = 0;
    if (e->flag34) { e->ord10043(0x297, 0); e->flag34 = 0; }
}
