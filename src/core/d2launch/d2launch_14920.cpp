#include <cstdint>
// 1.13c D2Launch+0x14920 -- intro-video path probe. Register inputs: ESI = path buffer, EBX = format, EDI = resolution string.
// Tries "<DATA\LOCAL>\video\<language from D2Lang#10004>\<res>" (sprintf 0x101B, cdecl 5 args); if Storm#268 cannot open it,
// falls back to the CHI (D2Lang#10009 == 0) or ENG directory; a path that opens is closed again with Storm#253(handle).
// The handle slot is never primed by this function: it is stack residue unless Storm#268 writes it.
struct D2l14920Seams {
    void (*lang4)(void* out8, uint32_t zero);                                              // D2Lang#10004(&local0, 0)
    void (*sprintf5)(char* buf, const char* fmt, const char* a, const void* b, const void* c);  // 0x101B
    int (*open268)(uint32_t z0, const char* name, uint32_t z1, uint32_t* handle);          // Storm#268
    void (*close253)(uint32_t handle);                                                      // Storm#253
    int (*lang9)();                                                                         // D2Lang#10009
};
extern "C" void d2l_14920_core(char* buf, const char* fmt, const void* res, const char* dataLocal, const char* chi,
                               const char* eng, uint32_t handleResidue, const D2l14920Seams* s) {
    uint8_t local0[8] = {0};
    uint32_t local1 = handleResidue;
    s->lang4(local0, 0);
    s->sprintf5(buf, fmt, dataLocal, local0, res);
    if (s->open268(0, buf, 0, &local1)) { s->close253(local1); return; }
    s->sprintf5(buf, fmt, dataLocal, s->lang9() ? eng : chi, res);
    if (s->open268(0, buf, 0, &local1)) s->close253(local1);
}
