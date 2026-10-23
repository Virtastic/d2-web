// src/core/fog/fog_1c060.cpp -- Fog+0x1C060, CPU model-name text from CPUID fields.
#include <cstdint>
#include <cstring>

// 1.13c Fog+0x1C060
// Custom convention: EAX=model, EBX=type (0 original, 1 overdrive), EDX=brand id, stack (out, vendor 1 Intel/2 AMD/3 Cyrix, family).
// out is pre-filled with "{Unknown}" (10 bytes written); a match overwrites strlen+1 bytes of it.
void fog_sub_1C060(uint32_t model, uint32_t type, uint32_t brand, char* out, int vendor, int family) {
    memcpy(out, "{Unknown}\0", 10);
    auto set = [&](const char* s) { memcpy(out, s, strlen(s) + 1); };
    if (vendor == 1) {
        switch (brand) {
            case 1: set("Celeron"); return;
            case 2: case 4: set("Pentium III"); return;
            case 3: set("Pentium III Xeon"); return;
            case 8: set("Pentium 4"); return;
            case 0xe: set("Xeon"); return;
        }
        if (type == 0) {
            if (family == 5) { set(model < 4 ? "Pentium" : "Pentium MMX"); return; }
            if (family == 6) {
                if (model < 3) set("Pentium Pro");
                else if (model < 5) set("Pentium II");
                else if (model == 5) set("Pentium II or Celeron");
                else if (model == 6) set("Celeron");
                else if (model == 7) set("Pentium III");
                else set("Pentium III (no brand)");
            } else if (family == 0xf) set("Pentium 4 (no brand)");
        } else if (type == 1) {
            if (family == 5) set(model < 3 ? "Pentium OD586" : model == 3 ? "Pentium OD486" : "Pentium MMX OD586");
            else if (family == 6) set("Pentium II OD");
        }
    } else if (vendor == 2) {
        if (family == 4) set("Am486DX4 or Am5x86");
        else if (family == 5) set(model < 6 ? "K5" : model < 8 ? "K6" : model == 8 ? "K6-II" : "K6-III");
        else if (family == 6) {
            if (model < 3 || (model > 3 && model < 6)) set("Athlon");
            else if (model == 3) set("Duron");
            else if (model == 6) set("Athlon MP, XP, or Mobile");
            else if (model == 7) set("Duron Mobile");
        }
    } else if (vendor == 3 && type == 0) {
        if (family == 4) set("MediaGX");
        else if (family == 5) set(model < 4 ? "6x86" : "GXm");
        else if (family == 6) set("6x86MX");
    }
}
