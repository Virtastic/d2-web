// 1.13c D2Client+0xBF0D0 -- level-entry title panel: load the level's title cel, draw it for 0x78 frames, then release it. No arguments.
// gl block (pinned module globals): +0x00 [0x11BBFC] the player, +0x04 [0xFAD54] the title cel handle, +0x08 [0x12340C] the frame
// counter, +0x0C [0x123410] the last level id, +0x10 [0x11C288] (three palette-index bytes written at +0..+2), +0x14 [0xDBC48] the
// screen width, +0x18 [0xDBC4C] the screen height, +0x1C [0x11C0A0] the expansion flag, +0x20 the 13-byte quest-title table at
// 0xDBE84 (level dword, quest index dword, 5-byte name; count dword at +0x3C = 0xDBEA0), +0x40 the quest byte array at 0x115590.
// No player, or #10331(player) (0xC1F0) == 0: release the cel (0xC18A(h), 0xBF86(ECX = h, EDX = file, 0x7A, 0)) and zero its cell.
// Level = #10826(#10331(player)) (0xC1DE). A new level picks three palette indices with #10190 (0xD390: (0,0x80,0), (0xC0,0xC0,0), (0xFF,0,0)),
// releases the cel, restarts the counter (0 when a level was already shown, 0x78 on the first) and keeps the level. Counter >= 0x78:
// release and return. Else counter+1, rec = #10014(level) (0xC214), the act folder "ACT%d" from #10459 (0xC1EA) below level 0x6D,
// "EXPANSION" above, the language folder from #10004 (0xD1E0), the path "%s\UI\%s\%s\%s" with rec+0x145; a quest-title table row for
// the level (expansion flag set, index < 0x25, quest byte set) re-points it at "%s\UI\%s\ACT1\%s"; the flag clear or the index out of
// range on a matching row is Fog #10265(0x60 / 0x65), #10024, exit(-1). The cel loads through 0x2B420(EAX = path, 0) when the cell is
// empty, and draws by #10138 (0xD372: ECX = cel, EDX = width/2, height/2 - 0x8C, 1, 5, 1). Sprintf is 0x3146 (cdecl); the vector logs its first three words and ESI
// (rec, then rec+0x145), which is what the hook can see without stack residue.
#include <cstdint>
#include <cstring>
#include "cl_env.h"
namespace {
constexpr uint32_t kMod = 0xB2000000u;   // a retail module address as the call log records it
void wr8(const cl::Env& e, uint32_t a, uint32_t v) { uint32_t b = a & ~3u, s = (a & 3) * 8; e.W(b, (e.R(b) & ~(0xffu << s)) | ((v & 0xff) << s)); }
void release(const cl::Env& e, uint32_t cell) {
    uint32_t h = e.R(cell);
    if (!h) return;
    e.H(0xc18a, h);
    e.H(0xbf86, 0x7a, 0, 0, 0, 0, 0, h, kMod | 0xd6ab0u);
    e.W(cell, 0);
}
void fatal(const cl::Env& e, uint32_t line) {
    uint32_t m = e.H(0xbf92, line);
    e.H(0xbf80, kMod | 0xd359fu, m);
    e.H(0x3664, 0xffffffffu);
}
}  // namespace
extern "C" void d2c_bf0d0(const cl::Env& e) {
    const uint32_t g = e.gl;
    const uint32_t pl = e.R(g);
    const uint32_t q0 = pl ? e.H(0xc1f0, pl) : 0;   // #10331(player): its result, not the player, is what #10826 takes (push eax at 0xBF127)
    if (!q0) { release(e, g + 4); return; }
    uint32_t lvl = e.H(0xc1de, q0);
    if (lvl != e.R(g + 0xc)) {
        wr8(e, g + 0x10, e.H(0xd390, 0, 0x80, 0));
        wr8(e, g + 0x11, e.H(0xd390, 0xc0, 0xc0, 0));
        wr8(e, g + 0x12, e.H(0xd390, 0xff, 0, 0));
        release(e, g + 4);
        uint32_t old = e.R(g + 0xc);
        e.W(g + 0xc, lvl);
        e.W(g + 8, old ? 0 : 0x78);
    }
    if ((int32_t)e.R(g + 8) >= 0x78) { release(e, g + 4); return; }
    e.W(g + 8, e.R(g + 8) + 1);
    uint32_t rec = e.H(0xc214, lvl);
    char lang[4] = {};
    char act[12] = {};
    char path[0x104] = {};
    const uint32_t pLang = (uint32_t)(uintptr_t)lang, pAct = (uint32_t)(uintptr_t)act, pPath = (uint32_t)(uintptr_t)path;
    if ((int32_t)lvl >= 0x6d) memcpy(act, "EXPANSION", 10);
    else {
        uint32_t a = e.H(0xc1ea, lvl) & 0xff;
        e.H(0x3146, pAct, kMod | 0xd34a8u, a + 1, 0, 0, 0, 0, 0, 0, rec);
    }
    e.H(0xd1e0, pLang, 0);
    uint32_t name = rec + 0x145;
    e.H(0x3146, pPath, kMod | 0xd348cu, kMod | 0xd349cu, 0, 0, 0, 0, 0, 0, name);
    int32_t n = (int32_t)e.R(g + 0x3c);
    for (int32_t i = 0; i < n; i++) {
        uint32_t row = g + 0x20 + 0xd * (uint32_t)i;
        if (lvl != e.R(row)) continue;
        if (!e.R(g + 0x1c)) { fatal(e, 0x60); return; }
        int32_t q = (int32_t)e.R(row + 4);
        if (q >= 0x25) { fatal(e, 0x65); return; }
        if (e.R8(g + 0x40 + (uint32_t)q)) {
            e.H(0x3146, pPath, kMod | 0xd3478u, kMod | 0xd349cu, 0, 0, 0, 0, 0, 0, (uint32_t)n);   // ESI is the table count here (loaded at 0xBF290); word 4 is the row's name (0xDBE8C + 13*i)
            break;
        }
    }
    uint32_t cel = e.R(g + 4);
    if (!cel) { cel = e.H(0x2b420, 0, 0, 0, 0, 0, pPath); e.W(g + 4, cel); }
    int32_t wd = (int32_t)e.R(g + 0x14), ht = (int32_t)e.R(g + 0x18);
    e.H(0xd372, (uint32_t)(ht / 2 - 0x8c), 1, 5, 1, 0, 0, cel, (uint32_t)(wd / 2));
}
