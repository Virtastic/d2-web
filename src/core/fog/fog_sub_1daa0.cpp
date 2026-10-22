// 1.13c Fog+0x1DAA0 (ordinal 10100): nested defer-scope counter. Stateful (global depth + pending list): NAMED, no FN vector.
#include <cstdint>

struct FogDeferNode {
    uint32_t handle;       // +0x3c
    uint32_t cur;          // +0x44
    uint32_t flags;        // +0x48
    uint32_t saved;        // +0x4c
    FogDeferNode* next;    // +0x50
};

// 1.13c Fog+0x1DAA0
extern "C"
void fog_sub_1daa0(int32_t enter, int32_t* depth, FogDeferNode** pending,
                   void (*apply)(uint32_t handle, uint32_t cur), void (*fatal)()) {
    if (enter) { ++*depth; return; }
    if (*depth > 0) {
        if (--*depth == 0) {
            for (FogDeferNode* n = *pending; n;) {
                FogDeferNode* next = n->next;
                if (n->cur != n->saved) apply(n->handle, n->cur);  // Ordinal_282
                n->flags &= 0xfffffff7u;
                n->saved = 0;
                n->next = nullptr;
                n = next;
            }
            *pending = nullptr;
        }
        return;
    }
    fatal();  // FogError(6, "", 0x216, "Unrecoverable internal error %08x"), _exit(-1)
}
