// 1.13c D2MULTI+0x07ED0 -- QueryInterface, ordinal 1, 6 bytes: `mov eax, &module_interface_table; ret`.
// Hands the caller the address of the module interface table (.data PTR_FUN_6f9eba74); slots stay pointers.
static void* g_multiModuleInterface[1];

extern "C"
void** d2multi_query_interface(void) {
    return g_multiModuleInterface;
}
