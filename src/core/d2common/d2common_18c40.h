// 1.13c D2Common 0x18C40 path builder and its two helpers 0x184C0 (line to points) and 0x18310 (collinear-point thinning).
#pragma once
#include <cstdint>
#include "d2common_186d0.h"
// 0x184C0: stdcall (p0, p1, maxLen, &dir), EDI = out points (x word, y word); returns the count, writes a zero x word after it.
extern "C" int32_t D2Common_184C0_LineToPoints(uint32_t p0, uint32_t p1, int32_t maxLen, int32_t* dirOut, uint16_t* out);
// 0x18310: stdcall (start, out, n), EDX = points; returns the number of points kept.
extern "C" int32_t D2Common_18310_ThinPoints(const uint32_t* pts, uint32_t start, uint32_t* out, int32_t n);
// 0x18C40: ECX = path info; 0x4CE90 comes from env->chk (a hook recorder in the FN probe).
extern "C" int32_t D2Common_18C40_BuildPath(uint32_t self, const D2CommonWalkEnv* env);
