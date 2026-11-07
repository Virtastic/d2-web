#include "../src/core/monspawn/item_flag_lines.h"

#include <cassert>

int main() {
    using d2::item_text::flagLines;
    const std::string E = "Ethereal (Cannot be Repaired)", S = "Socketed", sp = " ";
    assert(flagLines(0x10, 0, E, S, sp).empty());
    assert(flagLines(0x10 | 0x400000, 0, E, S, sp) == "\x01" "3Ethereal (Cannot be Repaired)");
    assert(flagLines(0x10 | 0x800, 2, E, S, sp) == "\x01" "3Socketed (2)");
    assert(flagLines(0x10 | 0x800 | 0x400000, 2, E, S, sp) == "\x01" "3Ethereal (Cannot be Repaired), Socketed (2)");
    assert(flagLines(0x800, 0, E, S, sp) == "\x01" "3Socketed (0)");
    return 0;
}
