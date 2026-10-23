#pragma once
#include <cstdint>
// Inputs of D2Common+0x1E920 made explicit: image globals 0xA0BA8 (slots), 0xA1488 (inited), 0xA13A0 (last),
// items.txt 0x9FB94/98, itemtypes count/stride/array at sgptDataTables+0xBFC/0xC00/0xC04.
struct D2Common1E920Env {
    int32_t* slots; int32_t* inited; int32_t* last;
    int32_t itemCount; const uint8_t* items;
    int32_t typeCount; uint32_t typeStride; const uint8_t* typeRows;
};
void D2Common_1E920_BuildCompositeTable(const D2Common1E920Env& e);
