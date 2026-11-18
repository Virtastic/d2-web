// 1.13c D2MULTI+0x07ED0 -- QueryInterface, ordinal 1, 6 bytes: `mov eax, &module_interface_table; ret`.
// Hands the caller the address of the module interface table (.data 0x1BA74). Retail's table holds one entry, slot 0 =
// D2Multi+0xE730 (the multiplayer screen entry); every other word is zero.
#include <cstdint>

extern "C" int32_t d2_multi_sub_e730(uint8_t*);

extern "C" {
void* d2multi_module_interface[16] = { (void*)&d2_multi_sub_e730 };

void** d2multi_query_interface(void) {
    return d2multi_module_interface;
}
}
