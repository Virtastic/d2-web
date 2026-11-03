// 1.13c Storm+0x34150: forget a window key (EDI): clear the display root when it matches, then find the key's record in the
// group list at 0x558E8 and, if present, send the four SEVT teardown commands (0x36020), unlink it (0x4D60) and free it (0x1B960).
#include <cstdint>

extern "C" void Storm_Sub34150Env(uint32_t edi, uint32_t* displayRoot, uint32_t head, void (*f_36020)(uint32_t, uint32_t),
                                  void (*f_4d60)(uint32_t), void (*f_free)(uint32_t, const char*, int32_t)) {
    if (edi == *displayRoot) *displayRoot = 0;
    uint32_t n = (int32_t)head < 1 ? 0 : head;
    if (0 < (int32_t)n) {
        while (*(uint32_t*)(uintptr_t)(n + 8) != edi) {
            n = *(uint32_t*)(uintptr_t)(n + 4);
            if ((int32_t)n < 1) return;
        }
        f_36020(0x534d5347, edi);
        f_36020(0x534d5348, edi);
        f_36020(0x534d534a, edi);
        f_36020(0x534d5349, edi);
        f_4d60(n);
        f_free(n, ".?AUWNDREC@@", -2);
    }
}
