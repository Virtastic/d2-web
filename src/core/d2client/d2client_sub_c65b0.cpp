// D2Client sub_C65B0, derived from the 1.13c decompile. Ordinals UNVERIFIED (guessed from call shape).
#include <cstdint>
#include <cstring>
struct ClientC65B0Env {
    int32_t (*o10980)(); int32_t (*o10649)();                   // D2Common #10980/#10649: x,y of EAX unit
    int32_t (*o10777)(int32_t v);                               // D2Common #10777 (velocity), result is 24.8 fixed
    int32_t (*o10080)(const void* edi, int32_t x, int32_t y);   // D2Common #10080 distance
    int32_t (*tbl)(int32_t i, int32_t f);                       // DAT_6fb85f54 + i*12 + f*4, f: 0 limit, 1 mul, 2 add
    void (*f77b00)(); void (*f77c80)(int32_t a, int32_t b);     // FUN_6fb77b00 / FUN_6fb77c80
};
static uint32_t rd(const uint8_t* p) { uint32_t v; memcpy(&v, p, 4); return v; }
// 1.13c D2Client+0xC65B0  eax=unit, edi=target, stack param_1
extern "C" void d2_client_c65b0(const void* eax, uint8_t* edi, const uint8_t* param1, const ClientC65B0Env& e) {
    if (!eax) return;
    uint32_t p = rd(param1 + 0x2c);
    if (!p) return;
    int32_t x = e.o10980(), y = e.o10649();
    int32_t v = e.o10777((int32_t)p);
    if ((v >> 8) == 0) return;
    int32_t d = e.o10080(edi, x, y);
    if (d < 1) d = 1;
    int32_t s = (int32_t)((uint32_t)(d << 16)) / ((v >> 8) << 12);
    if (s > 1) s--;
    int32_t i = 0;
    do { if (d < e.tbl(i, 0)) break; i++; } while (i < 1);   // loop bound: 0x6fb85f60 ends after one 12-byte entry
    int32_t t = e.tbl(i, 1) * d + e.tbl(i, 2);
    if (t < 500) t = 500;
    e.f77b00();
    uint32_t a = rd(edi + 0x54);
    if (a) { uint32_t b = rd((const uint8_t*)(uintptr_t)a + 0x30);
        if (b) { uint8_t* q = (uint8_t*)(uintptr_t)b; int32_t z = 0, n = -t; memcpy(q + 0x1c, &z, 4); memcpy(q + 0x20, &z, 4); memcpy(q + 0x24, &n, 4); } }
    e.f77c80(s, 0);
}
