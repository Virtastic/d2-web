// src/core/d2client/d2client_batch_c3dd0.h -- the D2Client 0x88D90 / 0xC3DD0..0xC4CE0 batch callee seam (shared with its probes).
#pragma once
#include <cstdint>
namespace d2client_batch_c3dd0 {
using P = uintptr_t;
struct Env {
 uint32_t (*rd32)(P); void (*wr32)(P, uint32_t);
 P dataTables;                       // *sgptDataTables
 P gCC268, gBB9CC, gBB9D0, gBB9D4, gBB9D8, gCBEFC, gC97F8; // DAT_6fbcc268, 6fbbb9cc.., 6fbcbefc, 6fbc97f8
 uint32_t (*tick)();                 // GetTickCount
 void (*writeFile)(P, const char*, uint32_t); // WriteFile after wvsprintfA
 uint32_t (*f39640)(uint32_t esi); uint32_t (*f38a70)(P, int, int, int); // 0x89640 (ESI = count), 0x88A70
 uint32_t (*o10511)(P); int32_t (*o10786)(P, uint32_t, uint32_t, uint32_t);
 void (*o10243)(uint32_t, uint32_t); uint32_t (*o10494)(P, uint32_t); void (*o10945)(P, uint32_t, uint32_t);
 uint32_t (*o10750)(P); uint32_t (*o10867)(P);
 P (*f5ff10)(P, P, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);
 uint8_t (*o10663)(); void (*o10706)(uint32_t, uint8_t); void (*o10520)(uint32_t, uint32_t);
 uint32_t (*o10331)(P, uint32_t, uint32_t); void (*o10591)(uint32_t, const uint32_t*, uint32_t); void (*o10052)(P*);
 P (*f630f0)(const uint32_t*); int32_t (*o10534)(P); void (*o10558)(P, int32_t); void (*o10970)(P, int32_t);
 void (*o10344)(P*);
 P (*f560d0)(P); void (*o11121)(P, P);
 void (*fada0b0)(); uint32_t (*o10265)(uint32_t); void (*o10024)(const char*, uint32_t); P (*o10392)(uint32_t);
 int32_t (*o10096)(P); uint32_t (*f1630)(P); uint32_t (*f1660)(P); void (*o10554)(uint32_t, uint32_t); void (*o10748)(uint32_t, uint32_t);
 uint32_t (*o10980)(uint32_t); uint32_t (*o10649)(uint32_t);
 int32_t (*f40550)(P); int32_t (*f51340)(int32_t*); P (*f515c0)(); P (*f51540)();
 void (*o10637)(P, int32_t); void (*o10019)(P, int32_t);
 int32_t (*o10080)(P, uint32_t, uint32_t); void (*o10425)(uint32_t, int32_t); void (*o10441)(P, int32_t); void (*o10837)(P, int32_t);
 uint32_t (*f38a70e)(uint32_t ebx, P, int, int, int); // 0x88A70 with its EBX operand (the 0x88D90 call)
};
uint32_t sub_88D90(const Env& e);
} // namespace d2client_batch_c3dd0
