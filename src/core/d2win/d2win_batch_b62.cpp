// Register-arg batch b62. Hidden register inputs are explicit leading parameters.
// 0x1B05 (write_string) skipped: its text source is not visible in the decompile.
#include <cstdint>
#include <cstdlib>
#include <cstring>

extern "C" {
__attribute__((weak)) uint32_t Ordinal_10265(uint32_t id);
__attribute__((weak)) void  Ordinal_10024(const void* s, uint32_t v);
__attribute__((weak)) int   Ordinal_10005(void* a, int b, int c);
__attribute__((weak)) int   Ordinal_10069(uint32_t v);
__attribute__((weak)) int   d2win_flsbuf(int ch, void* file);               // MSVC __flsbuf
}
extern "C" __attribute__((weak)) uint8_t* FUN_6f8ed790(void* self, void* self2, int a, void* b);   // 0xD790
extern "C" __attribute__((weak)) void FUN_6f8ebb00(uint8_t* a, char* b);   // 0xBB00
extern "C" __attribute__((weak)) uint32_t DAT_6f8ffe08[], DAT_6f8ffe5c[], DAT_6f8ffd48[], DAT_6f8ffdb0[];

namespace {
[[noreturn]] void fatal(uint32_t id) { Ordinal_10024("", Ordinal_10265(id)); std::exit(-1); }
inline uint32_t& u32(void* p, int off) { return *reinterpret_cast<uint32_t*>(static_cast<uint8_t*>(p) + off); }

struct MsvcFile { char* _ptr; int _cnt; char* _base; int _flag; };
}

// 1.13c D2Win+0x1aae  write_char: AL=ch, ECX=file, ESI=charsWritten*
void D2Win_1AAE(uint8_t al, MsvcFile* ecx, int* esi)
{
    if ((ecx->_flag & 0x40) == 0 || ecx->_base != nullptr) {
        uint32_t r;
        if (--ecx->_cnt < 0) r = d2win_flsbuf ? (uint32_t)d2win_flsbuf((char)al, ecx) : 0xffffffffu;
        else { *ecx->_ptr++ = (char)al; r = al; }
        if (r == 0xffffffffu) { *esi = -1; return; }
    }
    ++*esi;
}

// 1.13c D2Win+0x0c680  EAX=table ptr; returns the layout-entry pointer (EAX stays live; callers test it)
uint8_t* D2Win_C680(uint8_t* eax, int p1, int p2)
{
    if (!eax) fatal(0x3f5);
    uint8_t* r = eax + ((uint32_t)eax[1] * p1 + 9 + p2) * (uint32_t)eax[0] + eax[1] + 0x1c;
    if (!r) fatal(0x3fd);
    return r;
}

// 1.13c D2Win+0x0e0d0  EAX=src object, ESI=dst record
void D2Win_E0D0(uint8_t* eax, uint8_t* esi)
{
    u32(esi, 0x0c) = u32(eax, 0x1c);
    u32(esi, 0x10) = u32(eax, 0x20);
    uint32_t k = esi[5];
    if (!DAT_6f8ffe08 || !DAT_6f8ffe5c || !DAT_6f8ffd48 || !DAT_6f8ffdb0) __builtin_trap();  // weak seam: unlinked retail tables
    u32(esi, 0x1c) = DAT_6f8ffe08[k];
    u32(esi, 0x28) = DAT_6f8ffe5c[u32(eax, 0x24)];
    u32(esi, 0x18) = DAT_6f8ffd48[u32(eax, 0x1c)];
    u32(esi, 0x20) = 0x2074696c;
    if (uint32_t v = u32(eax, 0x38 + k * 4)) u32(esi, 0x20) = v;
    void* self = reinterpret_cast<void*>((uintptr_t)u32(eax, 0xb8));
    if (!self) fatal(0x4f7);
    uint8_t* tbl = !FUN_6f8ed790 ? nullptr : FUN_6f8ed790(self, self, (int)u32(eax, 0x1c), reinterpret_cast<void*>((uintptr_t)u32(eax, 0x20)));
    if (!tbl) fatal(0x4fb);
    uint8_t* e = tbl + 0x1c;
    for (uint32_t i = 0; i < tbl[0]; ++i, e += 9) {
        if (k > 0xf) fatal(0x503);
        if (*e == k) {
            u32(esi, 0x08) = 6 < (int)u32(esi, 0x0c);
            u32(esi, 0x24) = DAT_6f8ffdb0[u32(esi, 0x10)];
            if (!u32(esi, 0x20)) fatal(0x514);
            if (FUN_6f8ebb00) FUN_6f8ebb00(e, (char*)(e + 5));
            u32(esi, 0x28) = 0;
            break;
        }
    }
    if (!u32(esi, 0x1c)) fatal(0x520);
    if (!u32(esi, 0x18)) fatal(0x521);
    if (!u32(esi, 0x28)) fatal(0x522);
    if (!u32(esi, 0x20)) u32(esi, 0x20) = 0x20787878;
    if (!u32(esi, 0x24)) u32(esi, 0x24) = 0x20787878;
}

// 1.13c D2Win+0xE920  EBX=owner object (first param)
int D2Win_E920(uint8_t* ebx, uint8_t* p1, int p2, int* p3, uint8_t p4, uint32_t p5)
{
    if (!ebx) fatal(0x57c);
    int n = 0;
    if (*p1 == 0) return 0;
    for (int i = 0; ; ) {
        uint8_t b = reinterpret_cast<uint8_t*>((uintptr_t)(uint32_t)p2)[i];
        if (b >= 0x10) fatal(0x583);
        alignas(8) uint8_t rec[0x48]; std::memset(rec, 0, sizeof rec);
        u32(rec, 0x40) = p4;
        u32(rec, 0x34) = 0;
        u32(rec, 0x00) = p5;
        rec[5] = b;
        D2Win_E0D0(ebx, rec);
        u32(rec, 0x44) |= 2;
        if (Ordinal_10005 && Ordinal_10069 && Ordinal_10005(rec, 0, 1) && u32(rec, 0x3c)) {
            int w = Ordinal_10069(u32(rec, 0x3c)) + (int)u32(ebx, 0x14);
            if (*p3 < w) *p3 = w;
            ++n;
        }
        if (++i >= *p1) return n;
    }
}
