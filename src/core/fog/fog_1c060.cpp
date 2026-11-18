// src/core/fog/fog_1c060.cpp -- Fog+0x1C060, CPU model-name text from CPUID fields.
#include <cstdint>
#include <cstring>

// 1.13c Fog+0x1C060
// Custom convention: EAX=model, EBX=type (0 original, 1 overdrive), EDX=brand id, stack (out, vendor 1 Intel/2 AMD/3 Cyrix, family).
// out is pre-filled with "{Unknown}" (10 bytes written); a match overwrites strlen+1 bytes of it.
// Returns the EAX retail leaves: the model in EAX unless the copy path moved string words through EAX, in which case it is
// the last dword/word/byte of the name that went through EAX/AX/AL (merged over what EAX held before).
uint32_t fog_sub_1C060(uint32_t model, uint32_t type, uint32_t brand, char* out, int vendor, int family) {
    memcpy(out, "{Unknown}\0", 10);
    auto set = [&](const char* s) { memcpy(out, s, strlen(s) + 1); };
    auto d = [&](int k) { uint32_t v; memcpy(&v, out + k, 4); return v; };
    auto w = [&](uint32_t e, int k) { uint16_t v; memcpy(&v, out + k, 2); return (e & 0xFFFF0000u) | v; };
    auto b = [&](uint32_t e, int k) { return (e & 0xFFFFFF00u) | (uint8_t)out[k]; };
    const uint32_t m = model;
    if (vendor == 1) {
        switch (brand) {
            case 1: set("Celeron"); return d(0);
            case 2: set("Pentium III"); return d(4);
            case 4: set("Pentium III"); return d(8);
            case 3: set("Pentium III Xeon"); return b(d(4), 0x10);
            case 8: set("Pentium 4"); return w(m, 8);
            case 0xe: set("Xeon"); return m;
        }
        if (type == 0) {
            if (family == 5) { if (m < 4) { set("Pentium"); return d(0); } set("Pentium MMX"); return d(4); }
            if (family == 6) {
                if (m < 3) { set("Pentium Pro"); return d(0); }
                if (m < 5) { set("Pentium II"); return w(m, 8); }
                if (m == 5) { set("Pentium II or Celeron"); return d(0x10); }
                if (m == 6) { set("Celeron"); return d(4); }
                if (m == 7) { set("Pentium III"); return d(8); }
                set("Pentium III (no brand)"); return m;
            }
            if (family == 0xf) { set("Pentium 4 (no brand)"); return b(d(8), 0x14); }
        } else if (type == 1) {
            if (family == 5) {
                if (m < 3) { set("Pentium OD586"); return d(4); }
                if (m == 3) { set("Pentium OD486"); return w(d(0), 0xc); }
                set("Pentium MMX OD586"); return d(8);
            }
            if (family == 6) { set("Pentium II OD"); return w(d(0), 0xc); }
        }
    } else if (vendor == 2) {
        if (family == 4) { set("Am486DX4 or Am5x86"); return d(0xc); }
        if (family == 5) {
            if (m < 6) { set("K5"); return w(m, 0); }
            if (m < 8) { set("K6"); return b(m, 2); }
            if (m == 8) { set("K6-II"); return m; }
            set("K6-III"); return d(0);
        }
        if (family == 6) {
            if (m < 3) { set("Athlon"); return d(0); }
            if (m == 3) { set("Duron"); return d(0); }
            if (m < 6) { set("Athlon"); return w(m, 4); }
            if (m == 6) { set("Athlon MP, XP, or Mobile"); return m; }
            if (m == 7) { set("Duron Mobile"); return d(4); }
        }
    } else if (vendor == 3 && type == 0) {
        if (family == 4) { set("MediaGX"); return d(4); }
        if (family == 5) { set(m < 4 ? "6x86" : "GXm"); return m; }
        if (family == 6) { set("6x86MX"); return d(0); }
    }
    return m;
}
