// Implementation batch 01040: D2Game helpers at 0x34630-0x38ED0, written from the cached 1.13c decompiles
// (build-wasm/ghidra-d2game/decomp). Only functions whose callee arguments are all explicit in the decompile
// are written here. Callees are declared by retail RVA (rNNNNN) or ordinal (Ordinal_N), as batch 4ebe0 does;
// they bind when their owning rows land. Not vectored (RE-DONE only). Retail data pointers (message tags,
// string tables) are explicit parameters, because the wasm port has no retail image at those addresses.
#include <cstdint>
#include <cstdlib>

extern "C" {
// D2Common / D2Win ordinals (same signatures the other d2game batch files use)
uint32_t Ordinal_10265(uint32_t);
void Ordinal_10024(const void*, uint32_t);
int32_t Ordinal_11076(void*, int32_t*, int32_t*, int32_t);
// D2Game RVA callees
void r_33CE0(int8_t, int8_t);      // QUESTS_EventCallback seam, called with (1, 1)
void r_DFC10(void*);               // iterate helper, one 4-byte scratch argument
void r_34D00(int32_t, const void*); // quest message send: (target, string)
}

// 1.13c D2Game+0x34630
void QuestEventCallbackActiveAct_34630(void)
{
    r_33CE0(1, 1);
}

// 1.13c D2Game+0x34660
// EAX (the argument record) must be non-NULL; NULL is a fatal error printed with message 0x996 and the module tag.
void QUESTS_RequireEventArgs_34660(const void* pArgs, const void* moduleTag)
{
    if (pArgs == nullptr) {
        uint32_t msg = Ordinal_10265(0x996);
        Ordinal_10024(moduleTag, msg);
        std::_Exit(-1);
    }
    r_33CE0(1, 1);
}

// 1.13c D2Game+0x37860
// The decompile passes a 4-byte scratch to the iterate helper; the returned local is never written by it, so 0.
int32_t ReturnZeroAfterUnitIterate_37860(void)
{
    int32_t scratch = 0;
    int32_t result = 0;
    r_DFC10(&scratch);
    return result;
}

// 1.13c D2Game+0x38ED0
// Ecx = dx, EDX = handle, stack = radiusSq, x, y, and EAX = dy. Inside the radius, the collision ordinal tests the
// two points (x, y) and (x + dx, y + dy); the result is true when the ordinal reports no collision.
bool CircleTestWithCollisionOrdinal_38ED0(int32_t dx, void* handle, int32_t radiusSq, int32_t x, int32_t y, int32_t dy)
{
    if (handle == nullptr) return false;
    if (dx * dx + dy * dy > radiusSq) return false;
    int32_t a[2] = { x, y };
    int32_t b[2] = { x + dx, y + dy };
    return Ordinal_11076(handle, a, b, 4) == 0;
}

// 1.13c D2Game+0x35C20
// AL = stage 0..4, EDX = target. Stages 0, 1, 2 and 4 send texts[0], texts[1], texts[2], texts[3]; stage 3 and any
// other value send nothing. The four string pointers are the retail rodata entries at 0x6FD2DF90, DFA8, DFD4, DFF0.
void QUESTS_TextByStage_35C20(uint8_t stage, int32_t target, const void* const texts[4])
{
    switch (stage) {
    case 0: r_34D00(target, texts[0]); return;
    case 1: r_34D00(target, texts[1]); return;
    case 2: r_34D00(target, texts[2]); return;
    case 4: r_34D00(target, texts[3]); return;
    default: return;
    }
}
