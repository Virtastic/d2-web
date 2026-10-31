// Shared env for the panel press/release pair: D2Client+0xAA4E0 (panel_press) and D2Client+0xAA740 (panel_release).
// Globals are passed by pointer; each comment gives the retail absolute address and its RVA.
// Callees go through host(&call) in retail's register and stack order (see D2cPanelCall); the host returns eax.
#pragma once
#include <cstddef>
#include <cstdint>
#include <cstring>

struct D2cPanelCall { uint32_t rva, eax, ecx, edx, ebx, esi, edi; uint32_t s[4]; };
typedef uint32_t (*D2cPanelHost)(D2cPanelCall*);

struct D2cPanelGlobals {
    const int32_t* screenW;   // [0x6FB8BC48] RVA 0xDBC48  screen width
    const int32_t* screenH;   // [0x6FB8BC4C] RVA 0xDBC4C  screen height
    const int32_t* panelX;    // [0x6FBCB9A0] RVA 0x11B9A0 panel origin x
    const int32_t* panelY;    // [0x6FBCB9A4] RVA 0x11B9A4 panel origin y
    uint8_t* boxFlag;         // [0x6FBACDBC] RVA 0xFCDBC  byte: last press was in the box
    int32_t* boxSlot;         // [0x6FBACDBD] RVA 0xFCDBD  dword: slot index, -1 when none
    const int32_t* lastBand;  // [0x6FBACDD6] RVA 0xFCDD6  dword: band of the previous press
    uint8_t* soundFlag;       // [0x6FBACDD5] RVA 0xFCDD5  byte: set on the slot-match path
    const uint8_t* slotTab;   // [0x6FBACD8C] RVA 0xFCD8C  dword at slotTab + 5 * slot
    const int32_t* fcdcd;     // [0x6FBACDCD] RVA 0xFCDCD  dword: packet argument
    int32_t* target;          // [0x6FBAADD0] RVA 0xFADD0  dword: current target
    const int32_t* aad84;     // [0x6FBAAD84] RVA 0xFAD84
    const int32_t* aad90;     // [0x6FBAAD90] RVA 0xFAD90
    const int32_t* dd5d0;     // [0x6FB8D5D0] RVA 0xDD5D0  dword: must be 0x26 (checked before target writes)
    const int32_t* cb828;     // [0x6FBCB828] RVA 0x11B828
    int32_t* cc418;           // [0x6FBCC418] RVA 0x11C418
    int32_t* cc414;           // [0x6FBCC414] RVA 0x11C414
    const int32_t* f9e14;     // [0x6FBA9E14] RVA 0xF9E14
    const int32_t* f9e18;     // [0x6FBA9E18] RVA 0xF9E18
    const int32_t* cc3c4;     // [0x6FBCC3C4] RVA 0x11C3C4
};

struct D2cPanelEnv {
    D2cPanelHost host;
    D2cPanelGlobals g;
};

// Event record passed as the single stack argument. x and y are read as unsigned words (movzx).
// The handled path writes the dwords at +0x18 (1) and +0x1C (0) before BBE84.
struct D2cPanelEvent {
    uint8_t pad0[0x0C];
    uint16_t x;      // +0x0C
    uint16_t y;      // +0x0E
    uint8_t pad1[0x18 - 0x10];
    int32_t f18;     // +0x18
    int32_t f1c;     // +0x1C
};
static_assert(offsetof(D2cPanelEvent, x) == 0x0C, "event x");
static_assert(offsetof(D2cPanelEvent, y) == 0x0E, "event y");
static_assert(offsetof(D2cPanelEvent, f18) == 0x18, "event +0x18");
static_assert(offsetof(D2cPanelEvent, f1c) == 0x1C, "event +0x1C");

// One callee call: rva is the D2Client RVA, the named registers are loaded as retail does, s[] are stack words in push order.
inline uint32_t panelCall(const D2cPanelEnv* e, uint32_t rva, uint32_t eax = 0, uint32_t ecx = 0, uint32_t edx = 0,
                          uint32_t ebx = 0, uint32_t esi = 0, uint32_t edi = 0,
                          uint32_t s0 = 0, uint32_t s1 = 0, uint32_t s2 = 0, uint32_t s3 = 0) {
    D2cPanelCall c{rva, eax, ecx, edx, ebx, esi, edi, {s0, s1, s2, s3}};
    return e->host(&c);
}

// 0x88A70: sound submit, EBX = sound id, four zero stack words (callee cleans).
inline void panelSound(const D2cPanelEnv* e, uint32_t id) { panelCall(e, 0x88A70, 0, 0, 0, id); }

// 0x8A4D0 tail shared by the slide: EAX = [F9E18], ECX = caller value, ESI = [CC3C4], EDX = 0, one stack word.
inline void panelSlideTail(const D2cPanelEnv* e, uint32_t ecx, uint32_t stack0) {
    panelCall(e, 0x8A4D0, *e->g.f9e18, ecx, 0, 0, *e->g.cc3c4, 0, stack0);
}

// Slide shared by both functions. Zero rule (AAD84 == 0 and AAD90 == 0): CC418 = CC414 = 0, D1B0(ecx=0),
// 8A4D0(ecx=[F9E14], stack 0). Otherwise: v = -(W/4) (round toward zero), CC418 = v, CC414 = 1, D1B0(ecx=v),
// q = [F9E14]/4 (round toward zero), 8A4D0(ecx=[F9E14]-q, stack -q). Returns true for the zero rule.
inline bool panelSlide(const D2cPanelEnv* e) {
    const D2cPanelGlobals& g = e->g;
    if (*g.aad84 == 0 && *g.aad90 == 0) {
        *g.cc418 = 0;
        *g.cc414 = 0;
        panelCall(e, 0xD1B0, 0, 0);
        panelSlideTail(e, uint32_t(*g.f9e14), 0);
        return true;
    }
    const int32_t w = *g.screenW;
    const int32_t v = -(w / 4);
    *g.cc418 = v;
    *g.cc414 = 1;
    panelCall(e, 0xD1B0, 0, uint32_t(v));
    const int32_t f = *g.f9e14;
    const int32_t q = f / 4;
    panelSlideTail(e, uint32_t(f - q), uint32_t(-q));
    return false;
}

// Target clear: 0xBBF8C, then the 0x26 check (assert path: 0xBBF92 / 0xBBF80 / 0x3664), FADD0 = 0, and when the
// previous target was set: 0xD0B4 log, 0x3CC60 (EAX = 5), 0xAA380 and 0xBE2E0.
inline void panelClearTarget(const D2cPanelEnv* e, int32_t prev) {
    const D2cPanelGlobals& g = e->g;
    panelCall(e, 0xBBF8C);
    if (*g.dd5d0 != 0x26) {
        const uint32_t r = panelCall(e, 0xBBF92, 0, 0, 0, 0, 0, 0, 0x139B);
        panelCall(e, 0xBBF80, 0, 0, 0, 0, 0, 0, 0x6FB8359F, r, 0x139B);
        panelCall(e, 0x3664, 0, 0, 0, 0, 0, 0, 0xFFFFFFFFu);
    }
    *g.target = 0;
    if (prev != 0) {
        const uint32_t r = panelCall(e, 0xD0B4, 0, 0, 0, 0, 0, 0, 0x6FB80828);
        panelCall(e, 0x3CC60, 5, 0, 0, 0, 0, 0, r);
        panelCall(e, 0xAA380);
        panelCall(e, 0xBE2E0);
    }
}

// Event +0x18 / +0x1C and BBE84 (stdcall, one stack word = the event handle). Returns eax.
inline uint32_t panelBbe84(const D2cPanelEnv* e, D2cPanelEvent* ev, uint32_t evRef) {
    ev->f18 = 1;
    ev->f1c = 0;
    return panelCall(e, 0xBBE84, 0, 0, 0, 0, 0, 0, evRef);
}
