// Storm SNet shims (StormLib replaces Storm; the SNet provider table is never initialised in ours, so the
// "provider table not initialised" outcome of retail is the one a call reaches). MAPPED, no vector.
#include <cstdint>

// 1.13c Storm+0x13E10: SNet teardown; the return is unconditional 1 (side effects touch Storm-internal state only).
extern "C" int storm_sub_13e10() { return 1; }

// 1.13c Storm+0x13EB0: create-game setup. 0 for a null/empty name, a zero arg 8, or a null out pointer (arg 11);
// otherwise 0 as well: the provider table is not initialised (retail's 0x4b4 branch). Provider state is not ported.
extern "C" int storm_sub_13eb0(const char* name, const char*, const char*, const void*, uint32_t, uint32_t, uint32_t, uint32_t,
                               uint32_t a8, uint32_t, const char*, uint32_t* out) {
    (void)name; (void)a8; (void)out;   // every branch returns 0; the out store (session base - 1) is not modelled (no session)
    return 0;
}

// 1.13c Storm+0x118A0: SNetEnumProviders (ordinal 103), stdcall(callback). Two guard branches are reachable in
// ours, both returning 0 with SNet's error global (RVA 0x45388) and last error set: 0x57 (ERROR_INVALID_PARAMETER)
// for a null callback, 0x4b4 (ERROR_BAD_PROVIDER) for the provider table, which is never initialised here --
// StormLib replaces Storm and nothing calls SNetInitializeProvider. `err` is the vector's window on that global
// (the capture gdumps it and the probe writes it back). Retail then enumerates the provider list and calls the
// callback once per provider with (node[0], node+0x28, node+0xA8); provider state is not ported, so that path
// and its 0x1C580/0x1B960 copy are not modelled.
extern "C" int storm_sub_118a0(const void* cb, uint32_t* err) {
    *err = cb ? 0x4b4u : 0x57u;
    return 0;
}

// 1.13c Storm+0x14210: ten-argument form; forwards to 13EB0 with two zero words inserted after argument 3.
extern "C" int storm_sub_14210(const char* a0, const char* a1, const char* a2, const void* a3, uint32_t a4, uint32_t a5, uint32_t a6,
                               uint32_t a7, uint32_t a8, uint32_t* a9) {
    return storm_sub_13eb0(a0, a1, a2, a3, 0, 0, a4, a5, a6, a7, (const char*)(uintptr_t)a8, a9);
}
