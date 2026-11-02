// src/core/d2client/d2client_batch_bbc10.h -- the Env of d2client_batch_bbc10.cpp (its FN probes include it).
#pragma once
#include <cstdint>
#include <initializer_list>
namespace d2client_batch_bbc10 {
using P = uintptr_t;
struct Env {
 uint32_t (*rd32)(P); uint32_t (*rd16)(P); void (*wr16)(P, uint32_t); void (*wr32)(P, uint32_t);
 P dataTables;                                   // *sgptDataTables
 P gA218C, gCC390, gCC394, gC9858, gC9854;       // DAT_6fba218c, 6fbcc390, 6fbcc394, 6fbc9858, 6fbc9854
 P g9A0, g9A4, g8BC4C, gBBFC, gBBC8;             // DAT_6fbcb9a0, 6fbcb9a4, 6fb8bc4c, 6fbcbbfc, 6fbcbbc8
 uint32_t (*o10240)(P); uint32_t (*o11029)(P); uint32_t (*o10203)(uint32_t);
 int32_t (*o10786)(P, uint32_t, uint32_t, uint32_t, int32_t* aux);  // aux = stack local the caller reads back
 void (*o10231)(P, P, uint32_t, uint32_t, uint32_t); uint32_t (*o10985)(P);
 uint32_t (*o11139)(P, uint32_t); int32_t (*o10702)(); int32_t (*o10744)(P, int32_t);
 int32_t (*o10028)(P str); void (*o10150)(P str, uint32_t x, uint32_t a, uint32_t b, uint32_t z);   // 0xD35A fastcall; 0xD37E fastcall + 3 stack
 int32_t (*o10672)(); int32_t (*o10973)(uint32_t, uint32_t, uint32_t);
 void (*o11089)(int32_t, uint32_t, uint32_t, int32_t, uint32_t flag, P st);
 uint32_t (*o10265)(uint32_t); void (*o10024)(uint32_t msgTag, uint32_t); 
 int32_t (*o10212)(P, uint32_t); void (*o10431)(P, uint32_t, P, int32_t, uint32_t);
 uint32_t (*o10003)(uint32_t); uint32_t (*o10003b)(uint32_t, uint32_t);
 void (*usprintf)(uint32_t n, P buf, uint32_t fmt); void (*o10013)(int32_t, uint32_t, uint32_t, uint32_t);
 uint32_t (*bc430)(P);                           // caller supplies the BC430 register args
 uint32_t (*f1230)();
 void (*f1380)(P); P (*f560d0)(); P (*f68df0)(uint32_t); void (*f62e00)(); void (*f617c0)(P); void (*f61720)();
 void (*f69470)(); void (*f69650)(int32_t); void (*fb210)(uint32_t, int32_t, uint32_t, P, uint32_t, P);
 uint32_t (*f60000)(P); void (*f6b5b0)(P, P, int32_t, uint32_t, uint32_t); void (*f682b0)(P, int32_t, int32_t);
 void (*f39f00)(uint32_t, int32_t, int32_t); void (*f6b570)();
 void (*fatal)(uint32_t);
};
void sub_BC4E0(const Env& e, P str, uint32_t left, uint32_t right, uint32_t a, uint32_t b);   // EBX, ECX, EAX, stack a, b
uint32_t sub_BC720(const Env& e, P c, uint32_t a);   // ECX = c, EAX = a
} // namespace d2client_batch_bbc10
