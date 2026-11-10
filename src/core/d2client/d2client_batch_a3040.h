// src/core/d2client/d2client_batch_a3040.h -- D2Client 0xA3040..0xA4B00 (the pop-up menu block): retail's register
// conventions as explicit parameters (pointers are retail's 32-bit words); every callee goes through FnCallees.
#pragma once
#include <cstdint>
#include "fn_callees.h"
namespace d2client_batch_a3040 {
uint32_t sub_A3040(FnCallees& s, uint32_t e /*eax*/, uint32_t p1 /*stack*/);
void sub_A3160(FnCallees& s, uint32_t unit /*ecx*/);
void sub_A3220(FnCallees& s, uint32_t e /*edi*/);
void sub_A32D0(FnCallees& s, int32_t idx /*eax*/, uint32_t m /*edi*/);
void sub_A3340(FnCallees& s, uint32_t m /*eax*/, uint32_t v /*stack*/);
void sub_A3370(FnCallees& s, uint32_t m /*ecx*/, int32_t idx /*esi*/);
void sub_A3400(FnCallees& s, uint32_t m /*esi*/, uint32_t v /*stack*/);
void sub_A3580(FnCallees& s, uint32_t m /*eax*/);
void sub_A3A20(FnCallees& s);
void sub_A3AD0(FnCallees& s);
uint32_t sub_A4A90(FnCallees& s, uint32_t a /*eax*/, uint32_t b /*ecx*/, uint32_t target /*edi*/);
void sub_A4B00(FnCallees& s, uint32_t obj /*eax*/, uint32_t v /*stack*/);
}
