// Host test: g++ -o /tmp/t src/core/d2game/d2game_fwrite_321f.test.cpp && /tmp/t
#include <cstring>
#include "d2game_batch_6820.cpp"
#include <cassert>
int main() {
    FILE* f = std::tmpfile();
    assert(f);
    assert(D2Game_CrtFwrite("abcdef", 2, 3, f) == 3);  // returns element count, not bytes
    assert(D2Game_CrtFwrite("x", 1, 0, f) == 0);       // zero count writes nothing
    std::rewind(f);
    char b[8] = {};
    assert(std::fread(b, 1, 7, f) == 6 && !std::memcmp(b, "abcdef", 6));
    D2Game_CrtFclose(f);
    return 0;
}
