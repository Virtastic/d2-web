// 1.13c D2Multi+0xE840 (5 B): entry of the chat callback table.
// Returns 0 and pops its two stack words (ret 8); nothing is read or written.
#include "d2multi_env.h"
#include <cstring>

uint32_t d2multi_e840(const D2mEnv& e, uint32_t a, uint32_t b) {
    (void)e; (void)a; (void)b;
    return 0;
}
