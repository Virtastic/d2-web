#pragma once
#include <cstdint>
namespace d2client_batch_c88a0 {
using P = uintptr_t;
struct Env {
 uint32_t (*rd32)(P); uint32_t (*rd8)(P); void (*wr32)(P, uint32_t);
 P dataTables; P bitMasks;        // *sgptDataTables, gdwBitMasks
 P tblDX; P tblDY;                // DAT_6fb86dac, DAT_6fb86d8c (8 x int32)
 P tblAA420; uint32_t cb23b60;    // DAT_6fbaa420 byte table; callback addr
 int32_t (*f40550)(P, int32_t); /* eax=cls, stack=unit */ int32_t (*f51340)(uint32_t*); P (*f630f0)(const uint32_t*);
 P (*f515c0)(); uint32_t (*f1330)(P); int32_t (*f1260)(); P (*f1580)();
 void (*f52380)(uint32_t, int32_t); int32_t (*f55f70)(); void (*f38a70)(P, int, int, int);
 void (*f55420)(); P (*f58c20)(int); void (*f58be0)(P); void (*f77e30)(int32_t, int32_t, int32_t);
 void (*fda0b0)(); int32_t (*f55440)(); int32_t (*f51cf0)(int32_t, int32_t); void (*f1b300)(P, int, int, int, int);
 uint32_t (*f52c90)(P, int32_t, uint32_t, uint32_t);
 P (*o10511)(P); int32_t (*o10980)(P); int32_t (*o10649)(P); void (*o10243)(P, int); void (*o10554)(P, int);
 uint32_t (*o10867)(P); uint32_t (*o10750)(P); int32_t (*o10080)(); uint32_t (*o10192)(); void (*o10425)(uint32_t, uint32_t);
 int32_t (*o10786)(P, uint32_t, uint32_t, uint32_t); void (*o10440)(P, uint32_t); void (*o10074)(P, int32_t);
 void (*o10637)(P, uint32_t); void (*o10019)(P, int32_t); void (*o10520)(uint32_t, int); void (*o10441)(P, uint32_t);
 uint32_t (*o10392)(); uint32_t (*o10983)(); void (*o10764)(uint32_t); void (*o10286)(uint32_t, uint32_t, uint32_t);
 void (*o11129)(); int32_t (*o10494)(); void (*o10945)();
 void (*fatal)(uint32_t, const char*);
 void (*scale80)(uint32_t&, uint32_t&, uint16_t&); // x87 multiply by DAT_6fbaa438
};
uint32_t sub_C88A0(const Env& e, P a1, int32_t cls, uint32_t stackArg);
}
