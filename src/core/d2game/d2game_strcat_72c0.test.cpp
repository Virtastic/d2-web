// Host test: g++ -o /tmp/t src/core/d2game/d2game_strcat_72c0.test.cpp && /tmp/t
#include "d2game_batch_74eb.cpp"
#include <cassert>
extern "C" { void d2_fn_d2game_3b51(void) {} int32_t* d2game_errno_ptr() { return 0; } void d2game_unlock_3f76(int32_t) {} }  // link stubs for unrelated CRT fns in the batch file
#include <cstring>
int main() {
    for (int doff = 0; doff < 4; ++doff)
    for (int soff = 0; soff < 4; ++soff)
    for (int dl = 0; dl < 9; ++dl)
    for (int sl = 0; sl < 9; ++sl) {
        alignas(8) char db[64] = {}, sb[64] = {}, ref[64] = {};
        char* d = db + doff; char* s = sb + soff;
        memset(d, 'a', dl); memset(s, 'b', sl);
        memset(ref, 0, sizeof ref);
        memcpy(ref, d, dl); memcpy(ref + dl, s, sl);
        assert(d2_fn_d2game_72c0((uint8_t*)d, (const uint8_t*)s) == (uint8_t*)d);
        assert(memcmp(d, ref, dl + sl + 1) == 0);
    }
    return 0;
}
