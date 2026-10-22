// src/core/storm/storm_batch_b51.cpp -- Storm.dll teardown/ctor helpers, transcribed from the cached
// decompiles. Stage A batch lane: implementation + marker only. SEH frames omitted. Register-passed
// values (unaff_EDI) are explicit trailing parameters. Skipped: 0x29DF0 (reads uninitialised
// local_4 / ESI-implicit state, callee 0x195B0 convention unknown), 0x361C0 (callee 0x25FC0 takes
// an unrecovered register argument).
#include <cstdint>
#include <Windows.h>

void Storm_Sub3E20(uint32_t obj);
void Storm_Sub3EF0(uint32_t obj);
namespace {
uint8_t* at(uint32_t p, uintptr_t off) { return (uint8_t*)(uintptr_t)(p + off); }
uint32_t& u32(uint32_t p, uintptr_t off) { return *(uint32_t*)at(p, off); }
uint32_t& w(uint32_t a) { return *(uint32_t*)(uintptr_t)a; }

// Storm internals (seams, defined by their own rows)
void Storm_Free(uint32_t p, const char* file, int32_t line);           // 0x1B960
void Storm_Sub4B80(uint32_t obj);                                      // 0x4B80 (base dtor)
void Storm_Sub4D60(uint32_t node);                                     // 0x4D60 (list unlink)
void Storm_Sub1D460(int32_t a, int32_t b);                             // 0x1D460
void Storm_Sub36020(uint32_t tag, uint32_t wnd);                       // 0x36020
uint32_t* Storm_Sub1C580(uint32_t pool, const char* name, int32_t k, uint32_t flags); // 0x1C580
void Storm_Sub4390(void* z, uint32_t node, uint32_t edi);              // 0x4390
void Storm_Sub2FB30(const char* a, const char* b);                     // 0x2FB30

const char* kSfile = "..\\3rdParty\\STORM\\SOURCE\\SFILE.CPP";
const char* kSgdi = "..\\3rdParty\\STORM\\SOURCE\\SGDI.CPP";
const char* kScmd = "..\\3rdParty\\STORM\\SOURCE\\SCMD.CPP";

uint32_t Alloc4(uint32_t base, uint32_t off, const char* name, uint32_t flags, uint32_t edi) {
    uint32_t p = (uint32_t)(uintptr_t)Storm_Sub1C580(base + off, name, -2, flags | 8);
    if (p) { u32(p, 0) = 0; u32(p, 4) = 0; }
    if (edi) Storm_Sub4390(nullptr, p, edi);
    return p;
}
}

// 1.13c Storm+0x3E20
void Storm_Sub3E20(uint32_t p) {
    if (u32(p, 0x10c) != 0xffffffff) CloseHandle((HANDLE)(uintptr_t)u32(p, 0x10c));
    if (u32(p, 0x124)) Storm_Free(u32(p, 0x124), kSfile, 0xd6);
    if (u32(p, 0x130)) Storm_Free(u32(p, 0x130), kSfile, 0xd7);
    if (u32(p, 0x134)) Storm_Free(u32(p, 0x134), kSfile, 0xde);
    if (u32(p, 0x138)) Storm_Free(u32(p, 0x138), kSfile, 0xdf);
    Storm_Sub4B80(p);
}

// 1.13c Storm+0x3EF0
void Storm_Sub3EF0(uint32_t p) {
    if (u32(p, 0x10c) != 0xffffffff) CloseHandle((HANDLE)(uintptr_t)u32(p, 0x10c));
    if (u32(p, 0x110)) u32(u32(p, 0x110), 0x118) = 0;
    if (u32(p, 0x128)) Storm_Free(u32(p, 0x128), kSfile, 0x105);
    if (u32(p, 0x134)) Storm_Free(u32(p, 0x134), kSfile, 0x106);
    uint32_t q = u32(p, 0x110);
    if (q && --u32(q, 0x140) == 0) {
        Storm_Sub3E20(q);
        Storm_Free(q, "delete", -1);
    }
    Storm_Sub4B80(p);
}

// 1.13c Storm+0x3FC0
void Storm_Sub3FC0(uint32_t p) {
    uint32_t q = u32(p, 8);
    if (q && --u32(q, 0x140) == 0) {
        Storm_Sub3EF0(q);
        Storm_Free(q, "delete", -1);
    }
    if (u32(p, 0x3c) && u32(p, 0x44)) {
        uint32_t obj = u32(p, 0x3c);
        ((void(__cdecl*)(uint32_t))(uintptr_t)u32(w(obj), 8))(obj);   // virtual slot 2
    }
    Storm_Sub4B80(p);
}

// 1.13c Storm+0x41D0
uint32_t Storm_Sub41D0(uint32_t, uint32_t p2, uint32_t p3, uint32_t edi) {
    return Alloc4(p2, 0x158, ".?AUFILEREC@@", p3, edi);
}

// 1.13c Storm+0x4310
uint32_t Storm_Sub4310(uint32_t, uint32_t p2, uint32_t p3, uint32_t edi) {
    return Alloc4(p2, 0x50, ".?AUAUDIOSTREAM@@", p3, edi);
}

// 1.13c Storm+0x4C60
uint32_t Storm_Sub4C60(uint32_t, uint32_t p2, uint32_t p3, uint32_t edi) {
    return Alloc4(p2, 0x5c, ".?AUBITMAPREC@@", p3, edi);
}

// 1.13c Storm+0x17D60
uint32_t Storm_Sub17D60() {
    uint32_t n;
    while ((int32_t)(n = w(0x6FC45A14)) > 0) {
        Storm_Sub2FB30(*(int16_t*)at(n, 10) == 0 ? "HSGDIFONT" : "HSGDIOBJ", nullptr);
        if (*(int16_t*)at(n, 8) == 0x4f4d && *(int16_t*)at(n, 10) == 0) {
            if (w(0x6FC45824) == n) w(0x6FC45824) = 0;
            if (u32(n, 0xc)) { Storm_Free(u32(n, 0xc), kSgdi, 0x117); u32(n, 0xc) = 0; }
            Storm_Sub4D60(n);
            Storm_Free(n, ".?AUSGDIOBJ@@", -2);
        }
    }
    if (w(0x6FC45828)) { Storm_Free(w(0x6FC45828), kSgdi, 0x4b); w(0x6FC45828) = 0; w(0x6FC4582C) = 0; }
    if (w(0x6FC45814)) { Storm_Free(w(0x6FC45814), kSgdi, 0x135); w(0x6FC45814) = 0; w(0x6FC45818) = 0; w(0x6FC4581C) = 0; }
    return 1;
}

// 1.13c Storm+0x1D4D0
uint32_t Storm_Sub1D4D0() {
    w(0x6FC453B4) = 0;
    w(0x6FC453B0) = 0;
    w(0x6FC41A0C) = 0xffffffff;
    w(0x6FC41A10) = 0xffffffff;
    while ((int32_t)w(0x6FC459FC) > 0)
        Storm_Sub1D460(u32(w(0x6FC459FC), 8), u32(w(0x6FC459FC), 0xc));
    uint32_t n;
    while ((int32_t)(n = w(0x6FC45A08)) > 0) {
        Storm_Sub4D60(n);
        Storm_Free(n, ".?AUBITMAPREC@@", -2);
    }
    if (w(0x6FC453A8)) { Storm_Free(w(0x6FC453A8), "delete", -1); w(0x6FC453A8) = 0; }
    return 1;
}

// 1.13c Storm+0x24FD0
uint32_t Storm_Sub24FD0() {
    for (int i = 0; i < 2; i++) {
        uint32_t base = i == 0 ? 0x6FC459DC : 0x6FC459D0;
        for (int32_t it = (int32_t)w(base + 8); 0 < it; it = (int32_t)u32(it, 4)) {
            if ((u32(it, 8) & 0x30000) == 0x20000 && u32(it, 0x3c)) {
                Storm_Free(u32(it, 0x3c), kScmd, 0x212);
                u32(it, 0x3c) = 0;
            }
        }
    }
    uint32_t n;
    while ((int32_t)(n = w(0x6FC459E4)) > 0) { Storm_Sub4D60(n); Storm_Free(n, ".?AUCMDDEF@@", -2); }
    while ((int32_t)(n = w(0x6FC459D8)) > 0) { Storm_Sub4D60(n); Storm_Free(n, ".?AUCMDDEF@@", -2); }
    w(0x6FC45398) = 0;
    return 1;
}

// 1.13c Storm+0x34150
void Storm_Sub34150(uint32_t edi) {
    if (edi == w(0x6FC42FD0)) w(0x6FC42FD0) = 0;
    uint32_t h = w(0x6FC458E8);
    uint32_t n = (int32_t)h < 1 ? 0 : h;
    if (0 < (int32_t)n) {
        while (u32(n, 8) != edi) {
            n = u32(n, 4);
            if ((int32_t)n < 1) return;
        }
        Storm_Sub36020(0x534d5347, edi);
        Storm_Sub36020(0x534d5348, edi);
        Storm_Sub36020(0x534d534a, edi);
        Storm_Sub36020(0x534d5349, edi);
        Storm_Sub4D60(n);
        Storm_Free(n, ".?AUWNDREC@@", -2);
    }
}
