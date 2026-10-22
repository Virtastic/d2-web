// src/core/fog/fog_batch_b66.cpp -- 1.13c Fog.dll batch b66. Skipped (Win32/CRT/file-IO/threads or
// unrecovered helper callees): 0xD360 0x1A4A0 0x14D10 0x164A0 0x19530 0x1CB50.
#include <cstdint>
#include <cstring>
#include <initializer_list>

// 1.13c Fog+0x32F38 PERFORMANCE_FREQUENCY_TICKS_PER_MICROSEC
uint32_t g_fog_perf_ticks_per_usec;
// 1.13c Fog+0x32F3C PERFORMANCE_FREQUENCY_TICKS_PER_MILLISEC
uint32_t g_fog_perf_ticks_per_msec;
// 1.13c Fog+0x32F40 PERFORMANCE_ACCUMULATOR
uint64_t g_fog_perf_accumulator;
// 1.13c Fog+0x56E7C PERFORMANCE_ACCUMULATOR_INDEX
uint32_t g_fog_perf_accumulator_index;

// 1.13c Fog+0x0FDB0 -- hidden inputs: module_base (HMODULE, ebp; the caller resolves 0 to the exe),
// module_path (retail GetModuleFileNameA result, eax==0 means failure), out_section/out_offset
// (stack). addr is `this`. Walks the PE section table; section is 1-based, offset is addr-RVA.
// image points at the mapped module (module_base).
void fog_fdb0_section_of(const uint8_t* image, uint32_t addr_rva, int module_path_ok,
                         char* path, const char* module_path, int* out_section, int* out_offset) {
    strcpy(path, "*unknown*");
    *out_section = 0;
    *out_offset = 0;
    if (!module_path_ok) return;
    strcpy(path, module_path);
    if (!image) return;
    uint16_t mz; memcpy(&mz, image, 2);
    uint32_t lfanew; memcpy(&lfanew, image + 0x3C, 4);
    if (mz != 0x5A4D || !lfanew) return;
    uint32_t sig; memcpy(&sig, image + lfanew, 4);
    if (sig != 0x4550) return;
    uint16_t nsec, optsz;
    memcpy(&nsec, image + lfanew + 6, 2);
    memcpy(&optsz, image + lfanew + 0x14, 2);
    const uint8_t* s = image + lfanew + 0x18 + optsz;
    for (uint32_t i = 0; i < nsec; i++, s += 40) {
        uint32_t vsize, vaddr, rawsz;
        memcpy(&vsize, s + 8, 4); memcpy(&vaddr, s + 12, 4); memcpy(&rawsz, s + 16, 4);
        uint32_t len = rawsz > vsize ? rawsz : vsize;
        if (vaddr <= addr_rva && addr_rva <= len + vaddr) {
            *out_section = (int)i + 1;
            *out_offset = (int)(addr_rva - vaddr);
            return;
        }
    }
}

// 1.13c Fog+0x0FEB0 -- hidden inputs: code (eax), dst (ebx), n (edi) are explicit leading params.
// Retail falls back to FormatMessageA on NTDLL then "Unknown Exception"; the FormatMessage step is
// direct Win32 and is skipped here, so unlisted codes yield "Unknown Exception".
void fog_feb0_exception_name(uint32_t code, char* dst, size_t n) {
    static const struct { uint32_t c; const char* s; } t[] = {
        {0xc000008d, "FLT_DENORMAL_OPERAND"}, {0xc0000006, "IN_PAGE_ERROR"},
        {0x80000003, "BREAKPOINT"}, {0x80000001, "GUARD_PAGE"},
        {0x80000002, "DATATYPE_MISALIGNMENT"}, {0x80000004, "SINGLE_STEP"},
        {0xc0000005, "ACCESS_VIOLATION"}, {0xc0000008, "INVALID_HANDLE"},
        {0xc000001d, "ILLEGAL_INSTRUCTION"}, {0xc0000025, "NONCONTINUABLE_EXCEPTION"},
        {0xc0000026, "INVALID_DISPOSITION"}, {0xc000008c, "ARRAY_BOUNDS_EXCEEDED"},
        {0xc000008e, "FLT_DIVIDE_BY_ZERO"}, {0xc000008f, "FLT_INEXACT_RESULT"},
        {0xc0000090, "FLT_INVALID_OPERATION"}, {0xc0000091, "FLT_OVERFLOW"},
        {0xc0000092, "FLT_STACK_CHECK"}, {0xc0000093, "FLT_UNDERFLOW"},
        {0xc0000094, "INT_DIVIDE_BY_ZERO"}, {0xc0000095, "INT_OVERFLOW"},
        {0xc0000096, "PRIV_INSTRUCTION"}, {0xc00000fd, "STACK_OVERFLOW"},
    };
    const char* s = "Unknown Exception";
    for (auto& e : t) if (e.c == code) { s = e.s; break; }
    strncpy(dst, s, n);
}
