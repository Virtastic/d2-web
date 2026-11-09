// 1.13c D2Client+0xADB90 unit stat packet, written from the disassembly: ECX = packet {?, type, id dword, length, bits from +7}.
// The unit comes from 0xA4E20 (EAX = id & 0x7F, ECX = id, EDX = the per-type table 0x10A608 + type * 0x200, stack type). The bit
// reader (Fog 10126 init over packet+7, length-7; 10128 unsigned / 10129 signed reads) yields 8-bit stat-list ids until 0xFF: each
// list is opened (0x73760, EAX = unit), and when its 1-bit flag is set, 9-bit stat ids until 0x1FF are read against ItemStatCost
// (*sgptDataTables +0xBCC, count +0xBD4, 0x144-byte rows: +4 flag byte, +8 value bits, +9 param bits): the param read signed, the
// value signed when it is under 32 bits and the row's flag byte meets gdwBitMasks[1]'s low byte, else unsigned; 0x72610 (EAX = stat,
// ECX = list, EDX = unit; prev, value, param) chains its result. Then 0x72530 closes the list. A bad stat id ends the handler at once.
#include "cl_genv.h"
using namespace cl;
extern "C" void d2c13_adb90(uint32_t pk, const Env& e, const GEnv& g) {
    const uint32_t type = B(pk + 1), id = e.R(pk + 2);
    const uint32_t unit = e.H(0xa4e20, type, 0, 0, 0, 0, id & 0x7f, id, g.M(0x10a608 + type * 0x200));
    if (!unit) return;
    uint32_t bs[6] = {};
    const uint32_t b = Stk(bs);
    e.H(0xbf9e, b, pk + 7, B(pk + 6) - 7);
    uint32_t sid = e.H(0xbfb0, b, 8);
    if ((int32_t)sid >= 0xff) return;
    do {
        e.H(0x73760, sid, 0, 0, 0, 0, unit);
        if (e.H(0xbfb0, b, 1)) {
            uint32_t prev = 0, st = e.H(0xbfb0, b, 9);
            while ((int32_t)st < 0x1ff) {
                if ((int32_t)st < 0 || (int32_t)st >= (int32_t)e.R(e.dt + 0xbd4)) return;
                const uint32_t rec = e.R(e.dt + 0xbcc) + st * 0x144;
                if (!rec || !B(rec + 8)) return;
                const uint32_t ex = B(rec + 9) ? e.H(0xbfaa, b, B(rec + 9)) : 0;
                const uint32_t v8 = B(rec + 8);
                const uint32_t val = (v8 < 0x20 && (B(e.bm + 4) & B(rec + 4))) ? e.H(0xbfaa, b, v8) : e.H(0xbfb0, b, v8);
                prev = e.H(0x72610, prev, val, ex, 0, 0, st, sid, unit);
                st = e.H(0xbfb0, b, 9);
            }
        }
        e.H(0x72530, sid, 0, 0, 0, 0, unit);
        sid = e.H(0xbfb0, b, 8);
    } while ((int32_t)sid < 0xff);
}
