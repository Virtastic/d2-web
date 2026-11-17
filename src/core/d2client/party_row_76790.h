// 1.13c D2Client+0x76790: the colour mode of one party-panel row, shared by the port and the browser export.
#pragma once
#include <cstdint>
namespace d2cl_party {
// `share` = how many players hold node's party word, `pt` = node's party word, `mine` = the local party word
// (0xFFFF when not in a party of two or more), `hostile` = flag bit 8 on the local player's entry for the node.
// Mode 2 = my party, 8 = another party, 1 = hostile to me (it overrides), 0 = none.
static inline uint32_t rowMode(uint32_t share, uint16_t pt, uint16_t mine, bool hostile) {
    uint32_t mode = 0;
    if (share > 1 && pt != 0xffff) mode = (mine == pt) ? 2 : 8;
    if (hostile) mode = 1;
    return mode;
}
}
