// D2Multi's Fog free (#10043 through its import thunk): the one strong definition the D2Multi ports call. The generic weak
// Ordinal_10043 has more than one weak copy and the linker may keep any of them; a recorder sets g_d2m_free_hook to see
// D2Multi's frees, otherwise they go to Fog.
#include <cstdint>

extern "C" {
void Fog_Ordinal_10043(int32_t, int32_t);
void (*g_d2m_free_hook)(int32_t, int32_t) = nullptr;
void D2M_Fog_Free(int32_t line, int32_t z) {
    if (g_d2m_free_hook) g_d2m_free_hook(line, z);
    else Fog_Ordinal_10043(line, z);
}
}
