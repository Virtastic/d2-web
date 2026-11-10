// src/core/d2client/d2client_batch_a4b60.h -- D2Client 0xA52A0..0xA59E0 rewritten to retail's call sites: registers and stack
// words are explicit parameters (pointers are retail's 32-bit words); every callee not ported here goes through FnCallees.
// The "located" unit test shared by A5540/A5600/A5720/A57E0: type 2, 4 or 5 read the position record [unit+0x2C] directly
// (+0xC x, +0x10 y); every other type asks 0xC3BE / 0xC2EC (0 when the record is null).
#pragma once
#include <cstdint>
#include "fn_callees.h"
namespace d2client_batch_a4b60 {
uint32_t sub_A52A0(FnCallees& s, uint32_t target /*eax*/, uint32_t b /*stack*/);
void sub_A5360(FnCallees& s, uint32_t cb /*eax*/);
void sub_A5540(FnCallees& s, uint32_t unit /*esi*/);
void sub_A5600(FnCallees& s, uint32_t unit /*eax*/);
uint32_t sub_A5720(FnCallees& s, uint32_t unit /*eax*/, int32_t x /*ebx*/, uint32_t rect /*edi*/, int32_t y /*stack*/);
uint32_t sub_A57E0(FnCallees& s, uint32_t unit /*eax*/, int32_t x /*ebx*/, int32_t y /*stack*/);
void sub_A59E0(FnCallees& s, uint32_t unit /*esi*/, uint8_t b /*bl*/, uint8_t c /*stack*/);
}
