// Host test: mkdir -p /tmp/e && echo '#define EMSCRIPTEN_KEEPALIVE' > /tmp/e/emscripten.h && g++ -I/tmp/e -o /tmp/t src/core/d2game/crt_get_environment_strings_a_660e.test.cpp && /tmp/t
#include "crt_get_environment_strings_a_660e.cpp"
#include <cassert>
static const uint16_t W[] = {'A', '=', 'b', 0, 'C', 0, 0};
static const char A[] = "A=b\0C\0";
static int rel_w, rel_a, dealloc;
static const uint16_t* gw_ret = W;
static uint32_t err = 0;
static const uint16_t* gw() { return gw_ret; }
static const char* ga() { return A; }
static uint32_t le() { return err; }
static int32_t cv(const uint16_t* w, int32_t n, char* o, int32_t m) {
    if (!m) return n;
    for (int i = 0; i < n; i++) o[i] = (char)w[i];
    return n;
}
static void rw(const uint16_t*) { rel_w++; }
static void ra(const char*) { rel_a++; }
static void* al(uint32_t n) { return std::malloc(n); }
static void de(void* p) { dealloc++; std::free(p); }
int main() {
    D2GameEnvironment660e e{0, gw, ga, le, cv, rw, ra, al, de};
    char* r = (char*)d2game_environment_strings_660e(&e);
    assert(e.kind == 1 && r && !std::memcmp(r, A, 7) && rel_w == 1 && rel_a == 0);
    std::free(r);
    gw_ret = nullptr; err = 0x78; e.kind = 0;
    r = (char*)d2game_environment_strings_660e(&e);
    assert(e.kind == 2 && r && !std::memcmp(r, A, 7) && rel_a == 1);
    std::free(r);
    e.kind = 7;
    assert(!d2game_environment_strings_660e(&e));
    return 0;
}
