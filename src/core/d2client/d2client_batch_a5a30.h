// src/core/d2client/d2client_batch_a5a30.h -- D2Client 0xA5A30..0xA7D00 (the client unit hash tables and unit events):
// retail's registers and stack words as explicit parameters (pointers are retail's 32-bit words); every callee goes
// through FnCallees. Tables: 6 unit types x 0x80 buckets of 32-bit unit pointers at module 0x10A608 (live) and
// 0x109A08 (the unit flag 0xC4 bit 21 table); a unit's id is +0xC, its type +0, its bucket link +0xE4.
#pragma once
#include <cstdint>
#include "fn_callees.h"
namespace d2client_batch_a5a30 {
void sub_A5A30(FnCallees& s, uint32_t unit /*esi*/, uint32_t edi, uint32_t ebx, uint32_t p /*stack*/);
void sub_A5C50(FnCallees& s, uint32_t owner /*ebx*/, uint32_t flag /*stack*/);
uint32_t sub_A6500(FnCallees& s, uint32_t unit /*esi*/);
void sub_A6680(FnCallees& s);
void sub_A6F60(FnCallees& s, uint32_t unit /*esi*/, int32_t n /*eax*/, uint32_t kind /*ecx*/);
uint32_t sub_A6FC0(FnCallees& s, uint32_t x /*stack*/);
void sub_A7000(FnCallees& s);
void sub_A70C0(FnCallees& s, int32_t ev /*eax*/, uint32_t unit /*ecx*/);
void sub_A7D00(FnCallees& s, uint32_t edi);
}
