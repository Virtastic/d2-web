#include <cstdint>
#include <cstring>
// 1.13c Storm+0x0fef0: find node with byte[0x213]==id && byte[0x212]==0xFF in two lists.
struct StormEnvFef0 { uint8_t* list0; uint8_t* list1; };  // list0 = DAT_6fc45a3c, list1 = DAT_6fc45a64; head pointer at +8
extern "C" uint8_t* Storm_SubFEF0(int id, const StormEnvFef0* env) {
    if (id == 0xff) return nullptr;
    for (int i = 1; i >= 0; --i) {
        uint8_t* l = i ? env->list0 : env->list1;
        uint8_t* n; memcpy(&n, l + 8, sizeof n);
        while (n) {
            if (n[0x213] == (uint8_t)id && n[0x212] == 0xff) return n;
            memcpy(&n, n + 4, sizeof n);
        }
    }
    return nullptr;
}
