// Host test: tools/realm-tests.sh src/core/realm/client_refusal_sound.test.cpp
#include "client_refusal_sound.h"
#include <cassert>
#include <initializer_list>
int main() {
    using d2::realm::refusalSoundEvent;
    assert(refusalSoundEvent(4, 1) == 0x15);
    for (int st = 2; st <= 4; ++st) assert(refusalSoundEvent(4, st) == 0x13);
    assert(refusalSoundEvent(4, 0) == 0 && refusalSoundEvent(4, 5) == 0);
    for (int why : {0, 1, 2, 3, 5, 6, 7, 9}) assert(refusalSoundEvent(why, 1) == 0);
    return 0;
}
