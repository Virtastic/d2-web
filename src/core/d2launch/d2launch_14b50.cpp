// 1.13c D2Launch+0x14B50 (258 B): a screen-open entry of the launcher screen table (one unused stack word, ret 4).
// Sets the screen's draw routine slot (0x2552C) to the module routine 0x12A60, releases every image of the second
// loaded-image table (0x253C0, count at 0x25E34; the count is read again on every turn) through the thunk 0x9922 and
// empties it. Then loads three images (names at 0x238C0, 0x23E90, 0x23EC0; thunk 0x9952), appending each handle:
// the first goes to 0x995E (handle, 1) and 0x98B0 (handle, 1), the second and third to 0x98B0 (handle, 1). A count
// that is not below 64 (signed) before or after the second append takes the fatal path with string 0x60D. Last,
// 0x9976 runs with ECX = the third handle and EDX = the module string 0x1E228. Returns 1.
#include <cstdint>

struct D2l14b50Seams {
    void (*release)(uint32_t index);                 // thunk 0x9922, one stack word: the address of table[index]
    uint32_t (*load)(uint32_t name);                 // thunk 0x9952
    void (*first)(uint32_t handle, uint32_t one);    // thunk 0x995E
    void (*show)(uint32_t handle, uint32_t one);     // thunk 0x98B0
    void (*last)(uint32_t handle, uint32_t text);    // thunk 0x9976 (ECX, EDX)
    void (*fatal)(uint32_t text);                    // string lookup, error box, exit(-1)
};

extern "C" uint32_t d2l_14b50_core(int32_t* count, uint32_t* table, uint32_t* draw_slot, uint32_t draw, const uint32_t names[3],
                                   uint32_t text, const D2l14b50Seams* s) {
    *draw_slot = draw;
    if (*count > 0) {
        int32_t i = 0;
        do { s->release((uint32_t)i); ++i; } while (i < *count);
    }
    *count = 0;
    uint32_t h = s->load(names[0]);
    table[*count] = h; ++*count;
    s->first(h, 1);
    s->show(h, 1);
    bool full = true;
    if (*count < 0x40) {
        const uint32_t h2 = s->load(names[1]);
        table[*count] = h2; ++*count;
        s->show(h2, 1);
        full = !(*count < 0x40);
    }
    if (full) s->fatal(0x60D);
    h = s->load(names[2]);
    table[*count] = h; ++*count;
    s->show(h, 1);
    s->last(h, text);
    return 1;
}
