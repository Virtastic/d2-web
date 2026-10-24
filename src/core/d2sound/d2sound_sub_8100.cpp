// (alt copy; batch_67c0 carries the marker) D2sound 0x08100 -- for each of the 8 slots at 6f9c6560..6580: fatal if null or [0]==0; if [7]!=0 start fade (sub_6B80, 0, 200).
#include <cstdint>
void D2sound_sub_8100(int active, uint32_t* const slots[8], void (*fade6B80)(uint32_t*, uint32_t, int), void (*fatal)(int)) {
    if (!active) return;
    for (int i = 0; i < 8; i++) {
        uint32_t* p = slots[i];
        if (!p || !p[0]) { fatal(0x2a5); return; }
        if (p[7]) fade6B80(p, 0, 200);
    }
}
