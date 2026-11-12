// 1.13c D2Client+0x5DFD0: load the Sounds table (sounds.txt, 0x8e-byte records) and the SoundEnviron table (soundenviron.txt,
// 0x58-byte records), then the post-pass (group base at +0x60, leading Block count at +0x80, SoundEnviron min/max).
// The Fog table reads (#10016/#10208/#10210/#10207/#10209) are the d2moo compat FOG_* calls; the MPQ read is the shared VFS.
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <vector>
#include "../d2moo/compat/Fog.h"
#include "../tables/shared.h"
#include "d2client_5dfd0.h"
namespace d2client_sounds {
namespace {
struct Tbl { uint8_t* recs = nullptr; int32_t n = 0; uint8_t* env = nullptr; int32_t nEnv = 0; int32_t lo = 0, hi = 0; std::vector<uint32_t> map; };
Tbl T;
// Field tables, as 0x5DFD0 builds them on its stack: name, type (1 ascii, 2 dword, 3 word, 4 byte), length, offset.
D2BinFieldStrc kSounds[] = {
    {"Filename", TXTFIELD_ASCII, 0x3b, 0, nullptr}, {"Volume", TXTFIELD_BYTE, 0, 0x3c, nullptr}, {"Group Size", TXTFIELD_BYTE, 0, 0x3d, nullptr},
    {"Loop", TXTFIELD_BYTE, 0, 0x3e, nullptr}, {"Fade In", TXTFIELD_BYTE, 0, 0x3f, nullptr}, {"Fade Out", TXTFIELD_BYTE, 0, 0x40, nullptr},
    {"Defer Inst", TXTFIELD_BYTE, 0, 0x41, nullptr}, {"Stop Inst", TXTFIELD_BYTE, 0, 0x42, nullptr}, {"Duration", TXTFIELD_WORD, 0, 0x43, nullptr},
    {"Compound", TXTFIELD_WORD, 0, 0x45, nullptr}, {"Falloff", TXTFIELD_DWORD, 0, 0x47, nullptr}, {"Reverb", TXTFIELD_BYTE, 0, 0x4b, nullptr},
    {"Cache", TXTFIELD_BYTE, 0, 0x4c, nullptr}, {"Async Only", TXTFIELD_BYTE, 0, 0x4d, nullptr}, {"Priority", TXTFIELD_BYTE, 0, 0x4e, nullptr},
    {"Stream", TXTFIELD_BYTE, 0, 0x4f, nullptr}, {"Stereo", TXTFIELD_BYTE, 0, 0x50, nullptr}, {"Tracking", TXTFIELD_BYTE, 0, 0x51, nullptr},
    {"Solo", TXTFIELD_BYTE, 0, 0x52, nullptr}, {"Music Vol", TXTFIELD_BYTE, 0, 0x53, nullptr}, {"Block 1", TXTFIELD_DWORD, 0, 0x54, nullptr},
    {"Block 2", TXTFIELD_DWORD, 0, 0x58, nullptr}, {"Block 3", TXTFIELD_DWORD, 0, 0x5c, nullptr}, {"end", TXTFIELD_NONE, 0, 0, nullptr}};
D2BinFieldStrc kEnv[] = {
    {"Song", TXTFIELD_DWORD, 0, 0, nullptr}, {"Day Ambience", TXTFIELD_DWORD, 0, 4, nullptr}, {"Night Ambience", TXTFIELD_DWORD, 0, 8, nullptr},
    {"Day Event", TXTFIELD_DWORD, 0, 0xc, nullptr}, {"Night Event", TXTFIELD_DWORD, 0, 0x10, nullptr}, {"Event Delay", TXTFIELD_DWORD, 0, 0x14, nullptr},
    {"Indoors", TXTFIELD_BYTE, 0, 0x18, nullptr}, {"Material 1", TXTFIELD_DWORD, 0, 0x1c, nullptr}, {"Material 2", TXTFIELD_DWORD, 0, 0x20, nullptr},
    {"EAX Environ", TXTFIELD_DWORD, 0, 0x24, nullptr}, {"EAX Env Size", TXTFIELD_DWORD, 0, 0x28, nullptr}, {"EAX Env Diff", TXTFIELD_DWORD, 0, 0x2c, nullptr},
    {"EAX Room Vol", TXTFIELD_DWORD, 0, 0x30, nullptr}, {"EAX Room HF", TXTFIELD_DWORD, 0, 0x34, nullptr}, {"EAX Decay Time", TXTFIELD_DWORD, 0, 0x38, nullptr},
    {"EAX Decay HF", TXTFIELD_DWORD, 0, 0x3c, nullptr}, {"EAX Reflect", TXTFIELD_DWORD, 0, 0x40, nullptr}, {"EAX Reflect Delay", TXTFIELD_DWORD, 0, 0x44, nullptr},
    {"EAX Reverb", TXTFIELD_DWORD, 0, 0x48, nullptr}, {"EAX Rev Delay", TXTFIELD_DWORD, 0, 0x4c, nullptr}, {"EAX Room Roll", TXTFIELD_DWORD, 0, 0x50, nullptr},
    {"EAX Air Absorb", TXTFIELD_DWORD, 0, 0x54, nullptr}, {"end", TXTFIELD_NONE, 0, 0, nullptr}};
bool readAll(const char* path, std::vector<uint8_t>& b) { return d2::sharedVfs().read(path, b); }
uint8_t* parse(const char* path, D2BinFieldStrc* tbl, size_t recSize, int32_t* count) {
    std::vector<uint8_t> b;
    if (!readAll(path, b)) return nullptr;
    D2BinFileStrc* f = FOG_CreateBinFile(b.data(), b.size());
    *count = FOG_GetRecordCountFromBinFile(f);
    uint8_t* r = (uint8_t*)calloc((size_t)(*count ? *count : 1), recSize);
    FOG_10207(f, tbl, r, *count, recSize);
    FOG_FreeBinFile(f);
    return r;
}
}
// 1.13c D2Client+0x5DFD0 (fatal if either table is already loaded; here: a second call is a no-op)
bool load() {
    if (T.recs || T.env) return T.recs != nullptr;
    T.recs = parse("data\\global\\excel\\sounds.txt", kSounds, 0x8e, &T.n);
    T.env = parse("data\\global\\excel\\soundenviron.txt", kEnv, 0x58, &T.nEnv);
    if (!T.recs || !T.env) return false;
    int32_t base = 0; uint32_t grp = 1;
    int32_t lo = T.n, hi = 0;
    for (int32_t i = 1; i < T.n; ++i) {
        uint8_t* p = T.recs + 0xcb + (size_t)(i - 1) * 0x8e;
        int32_t v = (base == 0 || i < base || (int32_t)(base + grp) <= i) ? i : base;
        memcpy(p + 0x23, &v, 4);
        if (*p == 0) *p = 1; else { grp = *p; base = i; }
        uint32_t k = 0; uint8_t c = 0;
        do { int32_t bl; memcpy(&bl, p + 0x17 + k * 4, 4); if (bl == -1) break; c = (uint8_t)(c + 1); k = c; } while (c < 3);
        p[0x43] = (uint8_t)k;
    }
    for (int32_t e = 0; e < T.nEnv; ++e) {
        int32_t s; memcpy(&s, T.env + (size_t)e * 0x58, 4);
        if (s > 0) { if (s < lo) lo = s; if (s > hi) hi = s; }
    }
    T.lo = lo; T.hi = hi;
    T.map.assign((size_t)(hi - lo) + 1, 0);   // DAT_6fbc75b8
    return true;
}
uint8_t* records() { return T.recs; }
int32_t recordCount() { return T.n; }
uint8_t* envTable() { return T.env; }
int32_t envCount() { return T.nEnv; }
}
