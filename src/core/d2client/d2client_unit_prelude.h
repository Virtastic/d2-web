// The unit preamble shared by the D2Client 0x6B000.. helpers (1.13c D2Client+0x6C020 / 0x6BF30 / 0x6CB30 / 0x6D590): type = unit ? *unit : 6,
// id = unit ? unit[1] : -1, then D2Common #10175 (D2Client+0xC65E, one word) into res and #10350 (D2Client+0xC658, six words: unit, &type, &id,
// &res, file, line) which rewrites all three. The word order is the retail call site's; `file` is the module address of the source string.
#pragma once
#include "cl_env.h"
inline void d2cl_unit_prelude(const cl::Env& e, uint32_t unit, uint32_t fileRva, uint32_t line, int32_t* type, int32_t* id, int32_t* res) {
    *type = unit ? (int32_t)e.R(unit) : 6;
    *id = unit ? (int32_t)e.R(unit + 4) : -1;
    *res = (int32_t)e.H(0xc65e, unit);
    e.Xs(line);
    e.H(0xc658, unit, (uint32_t)(uintptr_t)type, (uint32_t)(uintptr_t)id, (uint32_t)(uintptr_t)res, 0x6FAB0000u + fileRva);
}
