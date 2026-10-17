// The edit-box key/character handler.
// Prior art: reference/D2MOO/source/D2Win/src/D2WinEditBox.cpp D2Win_10074 (1.10f 0x6F8A6F70, same
// signature and gate; the 1.10f body drove a real Win32 EDIT control and does not carry over).
// __fastcall(ECX = the control, DL = the byte / key code) -> 0 refused, 1 accepted.
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include "D2BitManip.h"

// The control prefix this handler reads, at the 1.13c offsets (the decompile's dword indices:
// [0]=+0x000 [2]=+0x008 [6]=+0x018 [0x11]=+0x044 [0x12]=+0x048 [0x15]=+0x054 [0x16]=+0x058
// [0x17]=+0x05C [0x97]=+0x25C [0x98]=+0x260 [0x9A]=+0x268 [0x9B]=+0x26C).
struct D2WinEditBox15450 {
    uint32_t nType;           // +0x000  D2WIN_EDITBOX == 1
    uint32_t unk004;          // +0x004
    uint32_t dwFlags;         // +0x008  controlHeader.dwFlags
    uint32_t unk00C[3];       // +0x00C
    uint32_t f018;            // +0x018
    uint32_t unk01C[10];      // +0x01C
    uint32_t f044;            // +0x044
    uint32_t nMaxLen;         // +0x048
    uint32_t unk04C[2];       // +0x04C
    uint32_t f054;            // +0x054
    uint32_t nTextLength;     // +0x058
    uint16_t wszText[256];    // +0x05C
    uint32_t nCaret;          // +0x25C
    uint32_t dwEditBoxFlags;  // +0x260
    uint32_t unk264;          // +0x264
    uint32_t pfnValidator;    // +0x268  int (*)(control, caret)
    uint32_t pfnChange;       // +0x26C  void (*)(int)
};
static_assert(offsetof(D2WinEditBox15450, dwFlags) == 0x008, "D2Win+0x15450 offset");
static_assert(offsetof(D2WinEditBox15450, f044) == 0x044, "D2Win+0x15450 offset");
static_assert(offsetof(D2WinEditBox15450, nTextLength) == 0x058, "D2Win+0x15450 offset");
static_assert(offsetof(D2WinEditBox15450, nCaret) == 0x25C, "D2Win+0x15450 offset");
static_assert(offsetof(D2WinEditBox15450, pfnValidator) == 0x268, "D2Win+0x15450 offset");
static_assert(sizeof(D2WinEditBox15450) == 0x270, "D2Win+0x15450 offset");

// The unmapped callees this handler calls, in call order: the port reports them instead of calling
// them (the retail capture tool/fn/README.md hooklog; the shape of src/core/fn/d2client_12dd0.cpp).
enum { EV_15450_137B0 = 0, EV_15450_13720, EV_15450_13770, EV_15450_13AC0 };
// D2Win+0x137B0 (UNMAPPED) is called on every accepted byte and gates +0x139A0 and the case 3/0x18
// blocks. the retail capture tool/fn/d2win-15450.txt pins it to 0 (`hook 0x137b0 0 -`), so its nonzero side is
// not ported and the covered paths are the 0 side.
static constexpr bool kKeyState137B0 = false;
// D2Win+0x6F9014A8: the callback re-entrancy counter (not part of the control).
static int32_t g_editBox15450Depth;

// D2Win+0x1C5C0 via D2Win+0x11880: the accepted-character table (8 dwords = 256 bits).
static const uint32_t kCharTable15450[8] = {
    0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, 0x7FFFFFFF, 0x00060000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
};
// D2Win+0x11880(dl): 0 refused. The table has bit 0xFF set; the 0xFF compare ahead of it clears it.
static bool d2win_15450_CharOk(uint32_t ch) {
    if (ch == 0xFF) return false;
    return (kCharTable15450[(ch >> 5) & 7] & gdwBitMasks[ch & 31]) != 0;
}

// 1.13c D2Win+0x15450
extern "C" uint32_t d2win_editbox_key(D2WinEditBox15450* p, uint32_t ch, uint32_t* ev, uint32_t* nev) {
    uint32_t n = 0;
    if (p == nullptr || p->nType != 1) {
        // 1.13c: Ordinal_10265(file) + Ordinal_10024 + _exit(-1); no vector can reach it.
        abort();
    }
    // gdwBitMasks_exref+8 and +0: the control must be alive and visible.
    if ((p->dwFlags & gdwBitMasks[2]) == 0 || (p->dwFlags & gdwBitMasks[0]) == 0) { *nev = 0; return 0; }

    if (ch == 9 && (p->dwEditBoxFlags & 0x4) != 0) {
        // VK_TAB with a validator: it is offered the caret, the text is left alone, always 0.
        if (p->pfnValidator != 0) {
            ++g_editBox15450Depth;
            ((int32_t (*)(D2WinEditBox15450*, uint32_t))(uintptr_t)p->pfnValidator)(p, p->nCaret);
            --g_editBox15450Depth;
        }
        *nev = 0;
        return 0;
    }

    const bool ok = d2win_15450_CharOk(ch) || ((p->dwEditBoxFlags & 0x20) != 0 && ch == 0xFF) ||
                    ((p->dwEditBoxFlags & 0x08) != 0 && ch == 0x0D) || ((p->dwEditBoxFlags & 0x10) != 0 && ch == 0x0E) ||
                    ch == 0x16 || ch == 3 || ch == 0x18 || ch == 1;
    if (!ok) { *nev = 0; return 0; }
    // The 0x16/3/0x18/1 keys are taken even if the validator refuses them.
    bool validated = true;
    if (p->pfnValidator != 0) {
        ++g_editBox15450Depth;
        validated = ((int32_t (*)(D2WinEditBox15450*, uint32_t))(uintptr_t)p->pfnValidator)(p, p->nCaret) != 0;
        --g_editBox15450Depth;
    }
    if (!validated && !(ch == 0x16 || ch == 3 || ch == 0x18 || ch == 1)) { *nev = 0; return 0; }

    ev[n++] = EV_15450_137B0;
    if (kKeyState137B0 && ch != 3 && ch != 0x18 && ch != 1) {
        abort();   // +0x139A0 is UNMAPPED and only reached from this side of the pin
    }

    // The text is edited in a 0x200-word copy: 0x200 default-constructed words, then strcpy.
    uint16_t buf[512];
    for (int32_t i = 0; i < 0x200; ++i) buf[i] = 0;
    uint32_t len = 0;
    while (p->wszText[len] != 0) { buf[len] = p->wszText[len]; ++len; }
    buf[len] = 0;
    uint32_t caret = p->nCaret;
    uint16_t* ins = buf + caret;

    switch (ch) {
    case 1:   // +0x13720(p): the whole text is taken, the caret is not moved here
        ev[n++] = EV_15450_13720;
        break;
    case 0x16:   // +0x138C0 returns the pending multi-byte string; UNMAPPED, so not ported
        abort();
    case 3:
    case 0x18: {   // a second +0x137B0; with the pin at 0 the block (and its +0x13800) is skipped
        ev[n++] = EV_15450_137B0;
        if (kKeyState137B0) abort();   // the block needs +0x13800 and a second unicode2Win
        break;
    }
    default:
        // retail's 0x137B0 call already happened above: these refusals keep it in the log
        if ((int32_t)p->nMaxLen - 1 <= (int32_t)len) { *nev = n; return 0; }
        if (caret > len) { *nev = n; return 0; }   // ponytail: no retail path, keeps the copy in bounds
        if ((int32_t)len < 0xff) {
            // unicode2Win/win2Unicode (D2Win imports at 0x6F8FB03C / 0x6F8FB04C) round-trip the tail
            // through the ANSI codepage; 0x00-0x7F and 0xA0-0xFF are the identity there, which is the
            // covered range (the vector's bytes and text are ASCII). The clamp that conversion takes
            // at 0x100 characters cannot be reached while nMaxLen <= 256.
            if (ch == 0xD) {   // "\r\n" is inserted as two characters
                const uint32_t tail = len - caret + 1;
                memmove(ins + 2, ins, tail * 2);
                ins[0] = 0xD;
                ins[1] = 0xA;
                ins += 2;
            } else {
                const uint32_t tail = len - caret + 1;
                memmove(ins + 1, ins, tail * 2);
                ins[0] = (uint16_t)ch;
                ins += 1;
            }
            ev[n++] = EV_15450_13770;
        }
        break;
    }

    const int32_t f11 = (int32_t)p->f044, f6 = (int32_t)p->f018;
    if ((p->dwEditBoxFlags & 0x8) != 0) {
        // 1.13c D2Win+0x14790 (UNMAPPED) measures the text against f6 - 2*f044; with f6 and f044 at 0
        // it faults on a null read. The set-bit half is not ported and no vector sets the bit.
        abort();
    }
    (void)f11; (void)f6;
    {
        uint16_t* d = p->wszText;
        const uint16_t* s = buf;
        do { *d++ = *s; } while (*s++ != 0);
        p->nCaret = (uint32_t)(ins - buf);
    }
    ev[n++] = EV_15450_13AC0;
    *nev = n;
    if (p->pfnChange != 0) ((void (*)(uint32_t))(uintptr_t)p->pfnChange)(0);
    return 1;
}
