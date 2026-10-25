// Storm batch b47-storm-s1: CRT string compare and the SEH-wrapped list unlink.
#include <cstdint>
void Storm_Sub4C20(uint8_t* node);

// 1.13c Storm+0x5C57: __stricmp, C-locale branch (retail locale block is zeroed -> ___ascii_stricmp):
// fold A-Z, return the difference of the first mismatching folded bytes (or of the NULs).
extern "C" int Storm_stricmp_5C57(const char* a, const char* b) {
    for (;;) {
        int x = (uint8_t)*a++, y = (uint8_t)*b++;
        if (x >= 'A' && x <= 'Z') x += 32;
        if (y >= 'A' && y <= 'Z') y += 32;
        if (x == 0 || x != y) return x < y ? -1 : x > y;   // retail returns -1/0/1
    }
}

// 1.13c Storm+0x4D60: SEH shim around 0x4B80 (unlink the {link,tag} node, run twice in retail; the
// second pass sees a cleared node and is a no-op), so one 0x4C20 unlink is identical.
void Storm_Sub4D60(uint8_t* node) { Storm_Sub4C20(node); }
