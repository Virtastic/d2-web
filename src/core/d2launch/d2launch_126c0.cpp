// 1.13c D2Launch+0x126C0 -- stdcall, three arguments (the third = text): accepts the first character when it is printable (0x21..0x7E as a signed char) and none of " # % * , / < > ? \; returns 1 for an accepted character, 0 otherwise.
#include <cstdint>
uint32_t d2launch_126c0(uint32_t, uint32_t, const char* text)
{
    const signed char c = text[0];
    switch (c) { case '"': case '#': case '%': case '*': case ',': case '/': case '<': case '>': case '?': case '\\': return 0; }
    return c > 0x20 && c < 0x7f;
}
