// Shared by the D2Client damage-range tooltip lines (0x34640, 0x35540): a (low, high) pair as wsprintfA text widened by toUnicode(dst, buf, 0x20).
//   low == high: "%d"; both < 10000: "%d-%d"; low < 10000 <= high: "%s%d-%dK" ("", low, (high+500)/1000); low >= 10000: "%s%dK-%dK".
// All comparisons are signed; the division truncates toward zero (retail's imul 0x10624DD3 / sar 6 / sign fix).
#pragma once
#include <cstdint>
#include "d2client_wfmt.h"
namespace d2client_pairfmt {
inline void putK(d2client_wfmt::Buf& o, int32_t v) { d2client_wfmt::putInt(o, (int32_t)((uint32_t)v + 500u) / 1000); }
inline void text(uint16_t* dst /* 32 zeroed chars */, int32_t p, int32_t q) {
    using namespace d2client_wfmt;
    Buf o;
    if (p == q) putInt(o, p);
    else if (p < 10000 && q < 10000) { putInt(o, p); put(o, '-'); putInt(o, q); }
    else if (p < 10000) { putInt(o, p); put(o, '-'); putK(o, q); put(o, 'K'); }
    else { putK(o, p); put(o, 'K'); put(o, '-'); putK(o, q); put(o, 'K'); }
    widen(o, dst);
}
}
