// Prior art used: D2MOO D2WinTextBox.cpp, D2Win_10049/10054/10056.
// Checked against cached 1.13c bodies 8290, 82D0 and 8110. These are the scalar
// fields of the existing control buffer used by d2win_sub_93C0; no pointer is
// decoded, narrowed or replaced. Callers retain ownership of that buffer.
#include <cstddef>
#include <cstdint>
#include "textbox_accessors113c.h"
#include <cstring>
#include <unistd.h>

namespace {
struct TextBoxScalarLayout {
    int32_t type;
    uint8_t preceding[0x54];
    int32_t line;
    uint32_t flags;
    int32_t currentLine;
    int32_t columns;
};
static_assert(offsetof(TextBoxScalarLayout, type) == 0);
static_assert(offsetof(TextBoxScalarLayout, line) == 0x58);
static_assert(offsetof(TextBoxScalarLayout, flags) == 0x5c);
static_assert(offsetof(TextBoxScalarLayout, currentLine) == 0x60);
static_assert(offsetof(TextBoxScalarLayout, columns) == 0x64);
int32_t read(const void* control, size_t offset) {
    int32_t value;
    std::memcpy(&value, static_cast<const uint8_t*>(control) + offset, sizeof value);
    return value;
}
void write(void* control, size_t offset, int32_t value) {
    std::memcpy(static_cast<uint8_t*>(control) + offset, &value, sizeof value);
}
void requireTextBox(const void* control) {
    // Retail's assertion reports through Fog and terminates with _exit(-1).
    if (!control || read(control, 0) != 4) _exit(-1);
}
}

// 1.13c D2Win+0x08290
extern "C" int32_t d2win_sub_8290(const void* control) {
    requireTextBox(control);
    return read(control, offsetof(TextBoxScalarLayout, currentLine));
}

// 1.13c D2Win+0x082D0
extern "C" int32_t d2win_sub_82D0(const void* control) {
    requireTextBox(control);
    return read(control, offsetof(TextBoxScalarLayout, line));
}

// 1.13c D2Win+0x08110
extern "C" void d2win_sub_8110(void* control, int32_t columns) {
    requireTextBox(control);
    if (columns < 1 || columns > 5 || read(control, 0x64) > columns) _exit(-1);
    write(control, 0x64, columns);
    if (columns > 1) write(control, 0x5c, read(control, 0x5c) | 0x20);
}
