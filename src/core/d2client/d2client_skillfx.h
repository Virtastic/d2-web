// src/core/d2client/d2client_skillfx.h -- callee seam of the D2Client client-side skill effect handlers
// 0xC45C0, 0xC4990, 0xC4A20, 0xC4B00, 0xC4C10, 0xC4CE0 (one port per file: d2client_<rva>.cpp). Each member is the
// retail callee named by its RVA (stdcall import thunks 0xC2DA.., module functions 0xAFF10..); register operands come first.
#pragma once
#include <cstdint>
namespace d2c_skillfx {
using P = uintptr_t;
using u32 = uint32_t;
using i32 = int32_t;
struct Env {
    u32 (*rd32)(P); void (*wr32)(P, u32); i32 (*rd16s)(P);
    P dt;                                               // *sgptDataTables (IAT slot 0xCE474)
    void (*fatal)(u32 line);                            // 0xBF92/0xBF80 + exit(-1): retail never returns
    u32 (*c310)(P);                                     // Ordinal 10511
    void (*a0b0)(P eax);                                // 0x2A0B0
    P (*c2da)(u32);                                     // Ordinal 10392
    i32 (*c6b2)(P);                                     // Ordinal 10096
    u32 (*f1630)(P ecx); u32 (*f1660)(P ecx);           // 0x1630 / 0x1660 (unit x / y)
    void (*cb50)(u32, u32); void (*cb7a)(u32, u32); void (*cb8c)(u32, u32);
    u32 (*c3be)(P); u32 (*c2ec)(P);
    u32 (*cb68)(u32); u32 (*cb6e)(u32);
    P (*aff10)(P edx, u32 idx, u32 x, u32 y, u32 cls, u32 s0);   // 0xAFF10 (ret 0x14)
    void (*f88a70)(u32 ebx, P n, u32, u32, u32);                 // 0x88A70 (ret 0x10)
    i32 (*f40550)(u32 eax, P u);                                 // 0x40550 (ret 4)
    u32 (*a1340)(P ebx, i32* edi, i32* s0);                      // 0xA1340 (ret 4): x through EDI, y through s0
    P (*a15c0)(P eax, u32 mi, u32 cls, u32 s0);                  // 0xA15C0 (ret 0xc)
    P (*a1540)(P eax, u32 idx, u32 cls, u32 s0);                 // 0xA1540 (ret 0xc)
    void (*c448)(P, i32); void (*c394)(P, i32);
    i32 (*c4d8)(P, i32, i32);
    u32 (*c610)(P); void (*c2b6)(u32, u32);
    i32 (*c430)(P); void (*c4a2)(P, i32); void (*c2c8)(P, i32); void (*c466)(P, i32);
    i32 (*c316)(P, u32, u32, u32); void (*c2fe)(P, i32);
    u32 (*c1f0)(P, u32, u32, P, u32, u32*);                      // Ordinal (stdcall 6): fills the 14-dword query
    void (*ccfa)(u32, u32*, u32); void (*ccd0)(u32*); void (*cce8)(u32*);
    P (*fb30f0)(u32* ecx);                                       // 0xB30F0: ECX = 23-dword request
};
u32 c45c0(const Env& e, P u, u32 p2, i32 p3);
u32 c4990(const Env& e, P a);
u32 c4a20(const Env& e, P a);
u32 c4b00(const Env& e, P a, i32 cls, u32 s0);
u32 c4c10(const Env& e, P u, i32 cls, u32 s0);
u32 c4ce0(const Env& e, P u, i32 cls, u32 s0);
} // namespace d2c_skillfx
