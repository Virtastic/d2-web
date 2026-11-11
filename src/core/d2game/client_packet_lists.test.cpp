// Host test: g++ -std=c++17 -o /tmp/t src/core/d2game/client_packet_lists.test.cpp && /tmp/t
#include "client_packet_lists.h"
#include <cassert>
#include <vector>
int main() {
    static d2g_lists::List l; l.clear();
    std::vector<std::vector<uint8_t>> seen;
    auto apply = [&](const uint8_t* p, int n) { seen.emplace_back(p, p + n); };
    uint8_t a[5] = {1, 2, 3, 4, 5}, b[3] = {0x3C, 9, 9}, big[0x201] = {};
    assert(l.append(a, 5) && l.append(b, 3));
    assert(!l.append(big, 0x201) && !l.append(a, 0) && !l.append(nullptr, 4));
    assert(seen.empty());                       // an append applies nothing
    d2g_lists::drainFrame(l, apply);            // the next frame applies both, in arrival order
    assert(seen.size() == 2 && seen[0].size() == 5 && seen[1][0] == 0x3C);
    d2g_lists::drainFrame(l, apply); assert(seen.size() == 2);   // and only once
    for (int i = 0; i < 300; i++) { bool ok = l.append(a, 5); assert(ok == (i < d2g_lists::kPackets)); }
    seen.clear(); d2g_lists::drainFrame(l, apply); assert(seen.size() == 256);
    assert(l.append(b, 3));                     // an empty list accepts again
    return 0;
}
