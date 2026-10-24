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

// 1.13c Storm+0xFDF0: SNet worker-thread stop. Retail signals and joins the thread behind the handle global (RVA 0x55854),
// then clears it; ours never starts one (handle is 0), so the guard is false and nothing happens.
extern "C" void storm_sub_fdf0() {}

// 1.13c Storm+0x11690: drains the two SNet message queues (list heads at RVA 0x55A3C / 0x55A64, count at +8) by calling
// sub_11630 while a count is positive. Ours never queues (counts stay 0), so both loops fall through.
extern "C" void storm_sub_11690() {}

// 1.13c Storm+0x11630: SNet queue teardown (register-passed list head): runs the message drain (0x115B0) 13 times, then frees
// the list object (0x52D0) and its allocation (0x1B960). Ours has no queues (see 11690), so every drain finds count 0 and there is no object to free.
extern "C" void storm_sub_11630() {}

// 1.13c Storm+0x14250: SNet receive pump. Loops on the provider's receive entry (provider table +0x2c, global RVA 0x45834)
// and dispatches each packet (header checks, ack/resend queues, 0x11160/0x11D90/0x12F40 handling) until it returns no packet.
// The provider table is never initialised in ours (see 118A0), so the first receive yields nothing and the loop exits at once.
extern "C" void storm_sub_14250() {}

// All five below: provider table never initialised in ours (DAT 0x45834 == 0), so after the argument guards (0x57) the
// outcome is retail's 0x4b4 branch: return 0, outputs as retail leaves them. Error global / last error not modelled.

// also Storm 0x16D90: SNetEnumGamesEx-style (ordinal 105): arg0 null or *arg0==0x24, callback non-null, else 0x57. Then
// walks the game list (RVA 0x55A50), empty in ours, and returns 1.
extern "C" int storm_sub_16d90(const uint32_t* a0, const void* cb) {
    if (!cb || (a0 && *a0 != 0x24)) return 0;
    return 1;
}

// also Storm 0x10720: ordinal 107 (query by id 1..6). Zeroes the buffer and *outLen first; no provider -> 0 (0x4b4).
extern "C" int storm_sub_10720(uint32_t, uint8_t* buf, uint32_t len, uint32_t* outLen) {
    if (buf) for (uint32_t i = 0; i < len; i++) buf[i] = 0;
    if (outLen) *outLen = 0;
    return 0;
}

// also Storm 0x10610: ordinal 108. 0x57 unless 1<=id<=4 and out non-null; then *out = 0 and 0 (0x4b4).
extern "C" int storm_sub_10610(uint32_t id, uint32_t* out) {
    if (!out || id == 0 || id > 4) return 0;
    *out = 0;
    return 0;
}

// also Storm 0x104F0: ordinal 109. Out min/max preset to (session base - 1); base is 0 in ours, so 0xFFFFFFFF; count 0; returns 0.
extern "C" int storm_sub_104f0(uint32_t* lo, uint32_t* hi, int32_t* n) {
    if (lo) *lo = 0xFFFFFFFFu;
    if (hi) *hi = 0xFFFFFFFFu;
    if (n) *n = 0;
    return 0;
}

// also Storm 0x16030: ordinal 110. Null out -> 0x57; else *out = 0 and 0 (0x4b4: no provider).
extern "C" int storm_sub_16030(int32_t* out) {
    if (out) *out = 0;
    return 0;
}
