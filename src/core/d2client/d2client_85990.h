// 1.13c D2Client+0x85990 and +0x82210 on real units (the realm client's own items).
#pragma once
#include <cstdint>
struct D2UnitStrc;
// 0x82210: the unit's (or a socketed item's) flag-0x40 stat list holding stat 0xCC under key (p2 << shift) + (p3 & mask) takes
// the charge count clamped to 0..its stored maximum. 1 when a list was found and rewritten.
int d2_client_charge_set_82210(D2UnitStrc* unit, int32_t p2, uint32_t p3, int32_t p4);
// 0x85990 for an item the client holds: `count` is the value read, `flag` the packet's flag bit, `arg` the 8/16-bit tail, `cmd` the stat id.
void d2_client_item_stat_85990(D2UnitStrc* hero, D2UnitStrc* item, int32_t count, char flag, uint16_t arg, int32_t cmd);
