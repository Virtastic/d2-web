// 1.13c BNClient+0xF3C0: pick the gateway name into the caller's out string.
// out[0] = 0; BNGatewayAccess::Load(&DAT_1E9F0); name = Nth(&DAT_1E9F0, DAT_1E9FC); Ordinal_501(out, name, max);
// if Ordinal_422 (SRegLoadString) answers non-zero, Ordinal_501(out, local, max) copies its scripted value too.
// Callees are injected: every one of them is a recorder in the capture, so the port only sequences the calls.
#include <cstdint>

struct BnF3C0Env {
    void (*load)(const void* gateway);                                                                   // 0x155D0, stdcall 1
    const char* (*nth)(const void* gateway, uint32_t index);                                             // 0x14CC0, thiscall, ret 4
    int32_t (*str_copy)(char* dst, const char* src, uint32_t max);                                       // 0x0804E, Ordinal_501, stdcall 3
    int32_t (*reg_load_string)(const char* key, const char* value, uint32_t flags, char* buf, uint32_t size);  // 0x08090, Ordinal_422, stdcall 5
    const void* gateway;   // &DAT_1E9F0 (module RVA 0x1E9F0)
    uint32_t index;        // DAT_1E9FC, the Nth index
    const char* key;       // module literal at RVA 0x1AD44
    const char* value;     // module literal at RVA 0x1AD50
};

extern "C" void bnclient_gateway_name_f3c0(const BnF3C0Env* e, char* out, uint32_t max) {
    out[0] = 0;
    e->load(e->gateway);
    const char* name = e->nth(e->gateway, e->index);
    e->str_copy(out, name, max);
    char local[0x100];
    if (e->reg_load_string(e->key, e->value, 0, local, sizeof local) != 0) e->str_copy(out, local, max);
}
