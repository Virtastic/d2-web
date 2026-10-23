// 1.13c Fog+0x14D10 (ordinal 10263): load and delete the "Crashdump" file, fold its records
// (4 dwords {a,b,c,n} + n dwords of stack) into a deduped table at `out`:
// byte count, then entries {u16 hits, u32 a, u32 b, u32 c, u8 n, n*4 bytes}.
// ponytail: crashdump path is bare "Crashdump" (retail prefixes the exe dir via sub_143D0/sub_17C40).
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

extern "C" int fog_sub_14d10(uint8_t* out, uint32_t* lenInOut)
{
    FILE* f = fopen("Crashdump", "rb");
    if (!f) return 0;
    if (fseek(f, 0, SEEK_END)) { fclose(f); return 0; }
    long n = ftell(f);
    fseek(f, 0, SEEK_SET);
    uint8_t* buf = (uint8_t*)malloc(n ? n : 1);
    if (!buf) { fclose(f); return 0; }
    fread(buf, 1, n, f);
    fclose(f);
    remove("Crashdump");
    uint32_t cap = *lenInOut;
    *lenInOut = 1;
    uint8_t* end = buf + n;
    uint8_t* p = buf;
    int count = 0;
    while (p + 16 <= end) {
        uint32_t h[4];
        memcpy(h, p, 16);
        uint32_t sz = h[3] * 4;
        if (p + 16 + sz > end || sz > 255 * 4) break;
        const uint8_t* st = p + 16;
        p += 16 + sz;
        uint8_t* e = out + 1;
        int i = 0;
        for (; i < count; i++, e += sz + 15) {
            uint16_t hits; uint32_t a, b, c;
            memcpy(&hits, e, 2); memcpy(&a, e + 2, 4); memcpy(&b, e + 6, 4); memcpy(&c, e + 10, 4);
            if (a == h[0] && b == h[1] && c == h[2] && e[14] == (uint8_t)h[3] && !memcmp(e + 15, st, sz)) {
                hits++; memcpy(e, &hits, 2);
                break;
            }
        }
        if (i == count) {
            if (e + 0x2f >= out + cap) break;
            uint16_t one = 1;
            memcpy(e, &one, 2); memcpy(e + 2, h, 12); e[14] = (uint8_t)h[3]; memcpy(e + 15, st, sz);
            count++;
            *lenInOut += sz + 15;
        }
    }
    out[0] = (uint8_t)count;
    free(buf);
    return 1;
}
