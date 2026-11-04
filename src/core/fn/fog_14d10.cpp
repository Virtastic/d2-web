// probe for Fog 0x14D10 (sub_14D10, fold the Crashdump file): regs ecx = out table (r[0]), edx = in/out byte count (r[1]); blobs out r[14], len r[16],
// cd r[18] (dword 0 = file size, then the file bytes: what retail's fread hook feeds the callee), buf r[20], lg r[22].
// The port reads a bare "Crashdump" through libc, so the probe writes the file (the case's bytes, or no file for the pinned GetFileAttributesA = -1 and
// fopen = 0 tuples: cd size 0xFFFFFFFF is not used, the file is absent when the descriptor's fopen hook returns 0, mirrored by cd dword 1 = 1) and calls the port.
// Retail's hook log (exe dir, fopen, fseek, ftell, allocator, fread, fclose) is not reproduced by the port: it has no seams for them.
#include <emscripten.h>
#include <cstdint>
#include <cstdio>
#include <cstring>
extern "C" int fog_sub_14d10(uint8_t* out, uint32_t* lenInOut);
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_fog_14d10(uint32_t* r) {
    const uint32_t* cd = (const uint32_t*)(uintptr_t)r[18];
    const uint32_t size = cd[0];
    remove("Crashdump");
    // the descriptor marks "no file" tuples with the fopen/attribute pins; the probe sees them as cd[1] == 0xFFFFFFFE
    if (cd[1] != 0xFFFFFFFEu) {
        FILE* f = fopen("Crashdump", "wb");
        if (f) { fwrite(cd + 1, 1, size, f); fclose(f); }
    }
    r[4] = (uint32_t)fog_sub_14d10((uint8_t*)(uintptr_t)r[0], (uint32_t*)(uintptr_t)r[1]);
    remove("Crashdump");
}
