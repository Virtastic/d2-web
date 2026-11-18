// 1.13c D2Multi+0x10200 (20 B): entry of the chat callback table.
// Shows the text (second stack word) in the chat pane with 0xFC90(text, 3), the error colour. The first stack word is not read. Returns 1; ret 8.
#include "d2multi_env.h"
#include <cstring>

uint32_t d2multi_10200(const D2mEnv& e, uint32_t a, uint32_t text) {
    (void)a;
    d2m_call(e, 0xfc90, 0, 0, 0, 0, 0, text, 3);
    return 1;
}
