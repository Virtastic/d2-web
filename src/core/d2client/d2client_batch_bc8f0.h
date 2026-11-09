// the Env of d2client_batch_bc8f0.cpp (its FN probes include it)
#pragma once
#include <cstdint>
namespace d2client_batch_bc8f0 {
using P = uintptr_t;
struct Env {
 uint32_t (*rd32)(P); uint32_t (*rd16)(P); void (*wr32)(P, uint32_t); void (*wr16)(P, uint32_t); void (*wr8)(P, uint32_t);
 uint32_t (*ord)(uint32_t id, P a, P b, P c, P d, P e, P f);   // Ordinal_<id> imports (D2Gfx/D2Win/D2Common/D2Lang)
 uint32_t (*fn)(P rva, P a, P b, P c, P d);                    // unrecovered D2Client callees by RVA
 int32_t (*uStrlen)(P); P (*uStrstr)(P, P); void (*uStrcpy)(P, P); void (*uStrncpy)(P, P, int32_t);
 void (*uSprintf)(int32_t, P, P); void (*win2Unicode)(P, P, int32_t); int32_t (*uWidth)(P, int32_t);
 int32_t (*sendMessageA)(uint32_t hwnd, uint32_t msg, uint32_t wp, uint32_t lp);
 int32_t (*getKeyState)(int32_t); void (*sprintfA)(P, P, uint32_t); int32_t (*snprintfPath)(P, P, P);
 void (*fatal)(uint32_t code);   // _exit(-1) path (Ordinal_10265 + 10024)
 P sgptDataTables, toUpperTable;
};
void sub_BE800(const Env& e, P s);
} // namespace d2client_batch_bc8f0
