// Host test: mkdir -p /tmp/e && echo '#define EMSCRIPTEN_KEEPALIVE' > /tmp/e/emscripten.h &&
//   g++ -I/tmp/e -o /tmp/t src/core/d2win/d2win_sub_97c0.test.cpp && /tmp/t
#include "d2win_batch_w2.cpp"
#include <cassert>
uint32_t d2win_g_6f901488, d2win_g_6f90148c;
static int calls, p3, p4, p5; static void* gbox; static uint16_t got[0x400];
static void add(void* b, const uint16_t* t, int a, int c, int d) { ++calls; gbox = b; memcpy(got, t, sizeof got); p3 = a; p4 = c; p5 = d; }
static uint32_t fcode;
static void fat(uint32_t c) { fcode = c; throw 1; }
int main() {
    int box;
    d2win_sub_97C0(&box, "A\xE9z", add, nullptr);          // bytes widen zero-extended, not sign-extended
    assert(calls == 1 && gbox == &box && p3 == 4 && p4 == 0 && p5 == 0);
    assert(got[0] == 'A' && got[1] == 0xE9 && got[2] == 'z' && got[3] == 0);
    char big[0x500]; memset(big, 'x', sizeof big - 1); big[sizeof big - 1] = 0;
    d2win_sub_97C0(&box, big, add, nullptr);               // capped at 0x400 units
    assert(got[0x3FF] == 'x');
    D2WinW2Env e{fat, 0, 0, 0};
    try { d2win_sub_97C0(nullptr, "a", add, &e); assert(0); } catch (int) {}
    assert(fcode == 0x109 && calls == 2);
    return 0;
}
