// src/core/fn/d2client_a4b60.cpp -- 1.13c D2Client+0xA4B60 replay probe.
// r[0] = obj (EAX). Null objects take retail's assert and _exit path, so they are not replayed.
#include <emscripten.h>
#include <cstdint>
#include "../d2client/d2client_a4b60_env.h"

extern "C" void d2_client_a4b60(uintptr_t eax, const D2ClientA4b60Env* e);

static uint32_t probe_rd32(uintptr_t a) { return *(volatile uint32_t*)a; }
static void probe_wr32(uintptr_t a, uint32_t v) { *(volatile uint32_t*)a = v; }
static void probe_fatal(uint32_t) {}

extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2client_a4b60(uint32_t* r) {   // r[0] arg in, r[4] result out
    D2ClientA4b60Env e{};
    e.rd32 = probe_rd32;
    e.wr32 = probe_wr32;
    e.fatal = probe_fatal;
    d2_client_a4b60((uintptr_t)r[0], &e);
    r[4] = 0;
}
