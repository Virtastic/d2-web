// src/core/d2client/d2client_batch_bea10.h -- the Env of d2client_batch_bea10.cpp (its FN probes include it).
#pragma once
#include <cstdint>
#include <cstddef>
#include <initializer_list>
namespace d2client_batch_bea10 {
using P = uintptr_t;
using W = uint16_t;
struct Env {
 uint32_t (*rd32)(P); void (*wr32)(P, uint32_t); uint32_t (*rd8)(P); void (*wr8)(P, uint32_t);
 // globals (retail addresses)
 P gBC48, gBC58, gB9C4, gB834, gBBFC, gAD54, gD3410, gD340C, gC288, gC4D4, gCC578, gCC778, gB9DC, gB9E0, gC991C;
 P gBEA0, gBE88, gC0A0, gC5590, gBC4C, g9264C, gCC28B, gB858, gB864, gB854, gCC3EC, gB92500, gD2496, gD24C0;
 P gC3AA4, gC3AB0, tblB834F0;           // tblB834F0 = first 10-byte record (base-4 of the Ghidra short*)
 P flagsA[6];                           // BEA10 must-be-zero flags: ADDC ADE4 ADE8 ADB0 ADF0 ADEC
 P flagsHalfL[8];                       // ADF4 ADD8 AD88 ADBC AE10 AE14 ADD0 (+pad) -> *out0 = w/2
 P flagsHalfR[2];                       // AD84 AD90 -> out2 = w/2
 // BEAF0 / BEB90 / BF370 / BF990
 P (*f2b420)(const char* path, P); void (*f143e0)(const uint8_t* buf, uint32_t len); /* 0x143E0: stack buf, EBX = length, ret 4 */ P (*f4d630)(P, uint32_t); int32_t (*f4d670)(P, uint32_t);
 bool (*f4dc60)(P, uint32_t, uint32_t); void (*toUnicode)(W*, P, uint32_t);
 bool (*f98c0)(P, uint32_t); uint32_t (*f8e50)(P, uint32_t);
 // ordinals
 P (*o10331)(P); int32_t (*o10826)(); uint8_t (*o10190)(uint32_t, uint32_t, uint32_t); void (*o10065)(P); void (*o10043)(uint32_t, uint32_t);
 P (*o10014)(int32_t); uint32_t (*o10459)(int32_t); void (*o10004)(char*, uint32_t); void (*o10138)(P handle, int32_t x, int32_t y, uint32_t, uint32_t, uint32_t);
 uint32_t (*o10265)(uint32_t); void (*o10024)(uint32_t, uint32_t);
 void (*o10184)(); void (*o10028)(); void (*o10150)(const W*, uint32_t, uint32_t, uint32_t);
 const W* (*o10003)(); int32_t (*o10973)(P, uint32_t, uint32_t); int32_t (*o10227)();
 uint32_t (*o10106)(); int32_t (*o10202)(); P (*o10695)(); int32_t (*o10744)(); int32_t (*o11088)();
 void (*o10085)(const W*, uint32_t, uint32_t); void (*o426)(const char*, const char*, uint32_t, uint32_t);
 void (*f55d90)(); void (*f414f0)(const W*, W*);
 void (*f24d80)(); void (*faed470)(); void (*faf6a20)(); void (*fb09740)(uint32_t); void (*fb2beb0)(P);
 int32_t (*spr)(char*, const char*, int32_t);  // _sprintf one %d
 void (*fatal)(uint32_t, const char*);
 void (*exitProc)(int32_t);  // CRT exit(-1) after a fatal message
};
bool sub_BEA10(const Env& e, int32_t* o);
void sub_BEAF0(const Env& e);
void sub_BEB90(const Env& e);
void sub_BF990(const Env& e, P a);
} // namespace d2client_batch_bea10
