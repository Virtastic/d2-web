// src/core/d2client/d2client_batch_afd10.h -- the Env of d2client_batch_afd10.cpp (its FN probes include it).
#pragma once
#include <cstdint>
namespace d2client_batch_afd10 {
using P = uintptr_t;
struct Env {
 uint32_t (*rd32)(P); void (*wr32)(P, uint32_t);
 P dataTables;            // *sgptDataTables
 uint32_t bitMask8;       // gdwBitMasks[8]
 P playerSelf;            // DAT_6fbcbbfc
 const int32_t* tbl83cb0; // 3 rows x 5 dwords: [count, 4 entries]
 const int32_t* tblY83cec; const int32_t* tblX83d0c; // 8 dwords each
 void (*o10034)(uint32_t ecx, uint32_t eax); void (*f3cb00)(P ebx);   // 0xD222 stdcall (ECX, EAX pushed); 0x3CB00 takes EBX = 0xAFC40
 int32_t (*o10985)(); int32_t (*o10191)(P); uint32_t (*o11029)(P); void (*o10240)(P, uint32_t);
 int32_t (*o10973)(P); void (*o10887)(P, int32_t, int32_t, int32_t);
 P (*f560d0)(uint32_t); P (*f630f0)(const uint32_t* block);  // B30F0 takes one 0x17-dword block
 uint32_t (*o10042)(uint32_t, uint32_t); void (*o10706)(uint32_t, uint32_t);
 int32_t (*o10867)(P); int32_t (*o10750)(P); uint32_t (*o10818)(P);
 void (*o10286)(P, int32_t, int32_t); void (*o10558)(P, int32_t); void (*o10970)(P);
 void (*o10336)(uint32_t, P); int32_t (*o10637)(); int32_t (*o10080)(); int32_t (*o10522)(); void (*o10334)(uint32_t);
 void (*o10739)(uint32_t, int32_t, int32_t, int32_t);
 int32_t (*o10910)(P); int32_t (*o10587)(); void (*o10614)();
 int32_t (*o10534)(P); uint32_t (*o10777)(uint32_t); void (*o10488)(int32_t, uint32_t, const char*, int32_t);
 void (*f77c80)(int32_t, int32_t); void (*f1380)(P); void (*f58be0)(P);
};
uint32_t sub_AFD10(const Env& e, uint32_t a, uint32_t b);   // a = ECX, b = EAX
} // namespace d2client_batch_afd10
