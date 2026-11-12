// 1.13c D2Common+0x47420: item property callback "add skill charges" (table entry). ecx unread, edx = the second
// register word, nine stack words (ret 0x24): the item, the property record, word 2 unread, word 3 = the stat,
// ..., words 6 and 7 passed on. The record must be non-null, the item a unit of type 4, the skill id (record
// +0x04) inside the skill table (data tables +0xBA0) and its record present (0x1250: eax = skill), else 0.
// Level (record +0x0C): zero derives it from the item level (0x23810: item) less the skill's required level
// (0x4EB00: skill), a quarter of that plus one, at least 1 and at most the skill's maximum (0x14D0: eax = skill);
// negative derives it as (item level - required level) / max(1, -((max(1, 99 - required)) / level)), at least 1.
// Charges (record +0x08): zero is 5; negative n is -n plus (-n * level) / 8; the result is clamped to 1..255.
// The current charges are a seeded roll (0x23C10: item gives the seed; 0x1100: ecx = seed, esi = charges less an
// eighth) plus that eighth plus 1. With the item's stat list (0x47020: eax = item, ecx = the edx word; stack:
// words 6 and 7) the stat is set (0x3A280: list, the stat as a signed 16-bit word, the low byte of the current
// charges + charges * 256, the skill id shifted by data tables +0xC6C plus the level masked by the 16-bit word at
// +0xC70). The result is the charges.
#pragma once
#include <cstdint>

namespace c47420 {
template <class E> uint32_t Apply(E& e, uint32_t dt, uint32_t d, uint32_t item, uint32_t rec, uint32_t s3, uint32_t s6, uint32_t s7)
{
    if (!rec) return 0;
    if (!item || e.rd32(item) != 4) return 0;
    const int32_t skill = (int32_t)e.rd32(rec + 4);
    if (skill < 0 || skill >= (int32_t)e.rd32(dt + 0xBA0)) return 0;
    if (!e.h(0x1250, (uint32_t)skill)) return 0;
    int32_t level = (int32_t)e.rd32(rec + 0xC);
    if (level == 0) {
        int32_t n = (int32_t)e.h(0x23810, 0, 0, 0, 0, 0, item);
        n -= (int32_t)e.h(0x4EB00, 0, 0, 0, 0, 0, (uint32_t)skill);
        n = n / 4 + 1;
        const int32_t mx = (int32_t)e.h(0x14D0, (uint32_t)skill);
        if (n <= 1) n = 1;
        level = n < mx ? n : mx;
    } else if (level < 0) {
        const int32_t req = (int32_t)e.h(0x4EB00, 0, 0, 0, 0, 0, (uint32_t)skill);
        int32_t a = 99 - req;
        if (a < 1) a = 1;
        int32_t div = -(a / level);
        if (div < 1) div = 1;
        const int32_t il = (int32_t)e.h(0x23810, 0, 0, 0, 0, 0, item);
        level = (il - req) / div;
        if (level <= 0) level = 1;
    }
    int32_t ch = (int32_t)e.rd32(rec + 8);
    if (ch == 0) ch = 5;
    else {
        if (ch < 0) { ch = -ch; ch += (int32_t)((uint32_t)ch * (uint32_t)level) / 8; }
        if (ch <= 1) ch = 1;
    }
    if (ch >= 0xFF) ch = 0xFF;
    const int32_t eighth = ch / 8;
    const uint32_t seed = e.h(0x23C10, 0, 0, 0, 0, 0, item);
    const uint32_t cur = e.h(0x1100, 0, seed, 0, 0, (uint32_t)(ch - eighth)) + (uint32_t)eighth + 1;
    const uint32_t list = e.h(0x47020, item, d, 0, 0, 0, s6, s7);
    if (list) {
        const uint32_t layer = ((uint32_t)skill << (e.rd32(dt + 0xC6C) & 31)) + (e.rd32(dt + 0xC70) & 0xFFFF & (uint32_t)level);
        e.h(0x3A280, 0, 0, 0, 0, 0, list, (uint32_t)(int32_t)(int16_t)s3, (cur & 0xFF) + ((uint32_t)ch << 8), layer);
    }
    return (uint32_t)ch;
}
}
