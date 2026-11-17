// 1.13c D2Client+0x4A060: the Resurrect row's insert / remove on one NPC menu record. The record is the 0x27-byte NPCMenu entry:
// count dword at +0x14, string words at +0x18, handler dwords at +0x22. Shared by the host-seam port (d2client_4a060.cpp) and the
// browser export (npc_menu_resurrect.cpp), so there is one implementation.
#pragma once
#include <cstdint>
#include <cstring>
namespace d2client_npcmenu {
inline uint32_t rd4(const uint8_t* p) { uint32_t v; std::memcpy(&v, p, 4); return v; }
inline void wr4(uint8_t* p, uint32_t v) { std::memcpy(p, &v, 4); }
inline void cp2(uint8_t* d, const uint8_t* s) { std::memcpy(d, s, 2); }
// rec -> the record (its +0x14 count, +0x18 words, +0x22 handlers). flag != 0 inserts, 0 removes.
inline void resurrect(uint8_t* rec, uint32_t flag, uint32_t fn46d10, uint32_t fn49440) {
    uint8_t* cnt = rec + 0x14; uint8_t* wd = rec + 0x18; uint8_t* hd = rec + 0x22;
    int32_t n = (int32_t)rd4(cnt);
    if (flag) {
        for (int32_t i = 0; i < n; ++i) if (rd4(hd + 4 * i) == fn46d10) return;
        int32_t at = -1;
        for (int32_t i = 0; i < n; ++i) if (rd4(hd + 4 * i) == fn49440) { at = i; break; }
        if (at >= 0) {
            for (int32_t k = n - 1; k >= at; --k) { wr4(hd + 4 * (k + 1), rd4(hd + 4 * k)); cp2(wd + 2 * (k + 1), wd + 2 * k); }
            wr4(cnt, rd4(cnt) + 1); wr4(hd + 4 * at, fn46d10); uint16_t w = 0x1507; std::memcpy(wd + 2 * at, &w, 2);
            return;
        }
        int32_t e = n - 1;
        wr4(hd + 4 * (e + 1), rd4(hd + 4 * e)); cp2(wd + 2 * (e + 1), wd + 2 * e);
        wr4(cnt, rd4(cnt) + 1); wr4(hd + 4 * e, fn46d10); uint16_t w = 0x1507; std::memcpy(wd + 2 * e, &w, 2);
        return;
    }
    for (int32_t i = 0; i < n; ++i) {
        if (rd4(hd + 4 * i) != fn46d10) continue;
        for (int32_t k = i; k < (int32_t)rd4(cnt) - 1; ++k) { wr4(hd + 4 * k, rd4(hd + 4 * (k + 1))); cp2(wd + 2 * k, wd + 2 * (k + 1)); }
        wr4(cnt, rd4(cnt) - 1);
        return;
    }
}
}
