// 1.13c D2Launch+0x16DE0 (153 B): a screen-open entry of the launcher screen tables (one unused stack word, ret 4).
// Runs the common background setup 0x1B310, then loads three more screen images (the names at 0x23EF0, 0x23F20 and
// 0x23F50, through the D2Win image loader thunk 0x9952) and appends each handle to the 64-entry loaded-image table
// (0x252B8, count at 0x25E30). A count that is not below 64 (signed) before the first or the second append reports
// string 0x624 through the fatal path; the third append is not guarded. Returns 1.
#include <cstdint>

struct D2l16de0Seams {
    void (*background)();              // D2Launch+0x1B310
    uint32_t (*load)(uint32_t name);   // D2Win image loader (thunk 0x9952)
    void (*fatal)(uint32_t text);      // string lookup, error box, exit(-1)
};

extern "C" uint32_t d2l_16de0_core(int32_t* count, uint32_t* table, const uint32_t names[3], const D2l16de0Seams* s) {
    s->background();
    bool full = true;
    if (*count < 0x40) {
        const uint32_t h = s->load(names[0]);
        table[*count] = h; ++*count;
        if (*count < 0x40) {
            const uint32_t h2 = s->load(names[1]);
            table[*count] = h2; ++*count;
            full = !(*count < 0x40);
        }
    }
    if (full) s->fatal(0x624);
    const uint32_t h3 = s->load(names[2]);
    table[*count] = h3; ++*count;
    return 1;
}
