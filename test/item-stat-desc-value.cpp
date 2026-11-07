#include "../src/core/monspawn/item_stat_desc_value.h"

#include <cassert>

int main() {
    using d2::item_text::descValue;
    // maxhp (stat 7, ValShift 8): a +25 life small charm holds 25 << 8 = 6400 and shows 25.
    assert(descValue(6400, 0, 0, 0, 0, 8) == 25);
    // maxmana +15 and maxstamina +40 (ValShift 8).
    assert(descValue(15 << 8, 0, 0, 0, 0, 8) == 15);
    assert(descValue(40 << 8, 0, 0, 0, 0, 8) == 40);
    // A stat without ValShift is unchanged (all resistances +5).
    assert(descValue(5, 0, 0, 0, 0, 0) == 5);
    // item_hp_perlevel (op 2, op param 3, op base level, ValShift 8) at level 50:
    // (50 * v) >> 3, then >> 8; retail rounds each shift down.
    assert(descValue(8, 2, 3, 50, 0, 8) == 0);
    assert(descValue(8 << 8, 2, 3, 50, 0, 8) == 50);
    // op 1 and op 6+ do not scale.
    assert(descValue(1 << 8, 1, 3, 50, 0, 8) == 1);
    assert(descValue(1 << 8, 6, 3, 50, 0, 8) == 1);
    // A negative value shifts arithmetically (-10 life).
    assert(descValue(-10 * 256, 0, 0, 0, 0, 8) == -10);
    return 0;
}
