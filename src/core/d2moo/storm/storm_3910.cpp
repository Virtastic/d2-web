// 1.13c Storm+0x3910: eh_vector_destructor_iterator over 16 elements of 0x14 bytes; each element dtor (Storm+0x3820) frees the buffer at [elem+8] ('.I'), last element first.
#include <cstdint>
extern "C" int32_t Storm_Sub1B960(void* p, const char* tag, int32_t line);
extern "C" void Storm13c_3910(uint32_t* obj) {
    for (int i = 15; i >= 0; i--) {
        uint32_t p = obj[i * 5 + 2];
        if (p != 0) Storm_Sub1B960((void*)(uintptr_t)p, ".I", -2);
    }
}
