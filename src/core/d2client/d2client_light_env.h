// Shared addresses and byte helpers for the light-grid ports that reach their state through cl::Env (retail 32-bit addresses).
// Byte loads and stores go through the aligned dword that holds the byte, so a pinned range never has to extend past its end.
#pragma once
#include <cstdint>
#include "cl_env.h"
namespace cllight {
constexpr uint32_t kBase   = 0x6FAB0000u;
constexpr uint32_t kSign   = kBase + 0xFCDE4u;    // row step, +1 or -1
constexpr uint32_t kLineA  = kBase + 0xFCDE8u;    // int32 line cache, 64 per row
constexpr uint32_t kLineB  = kBase + 0x100DE8u;   // int32 line overrides, 64 per row
constexpr uint32_t kFrac   = kBase + 0x104DE8u;   // blend weight, 0..256
constexpr uint32_t kDx     = kBase + 0x104DECu;   // column step, +1 or -1
constexpr uint32_t kOrgX   = kBase + 0x104DF0u;   // grid origin, in cells
constexpr uint32_t kOrgY   = kBase + 0x104DF4u;
constexpr uint32_t kCurve  = kBase + 0x104E00u;   // int32[256] channel scale by level
constexpr uint32_t kMode   = kBase + 0x105200u;   // step mode 0..4
constexpr uint32_t kGrid   = kBase + 0x105208u;   // 48 x 48 cells of 8 bytes
constexpr uint32_t kColour = kBase + 0xDE830u;    // coloured lighting on
constexpr uint32_t kDelay  = kBase + 0x11BBECu;   // delayed-release list head
inline uint32_t r8(const cl::Env& e, uint32_t a) { return e.R(a & ~3u) >> ((a & 3u) * 8) & 0xFFu; }
inline void w8(const cl::Env& e, uint32_t a, uint32_t v) {
    const uint32_t q = a & ~3u, sh = (a & 3u) * 8;
    e.W(q, (e.R(q) & ~(0xFFu << sh)) | (v & 0xFFu) << sh);
}
inline uint32_t cell(int32_t col, int32_t row) { return kGrid + (uint32_t)(row * 48 + col) * 8u; }
}  // namespace cllight
