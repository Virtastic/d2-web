#pragma once
#include "native_list.h"
#include <mutex>
using D2ClientMessageSource = int32_t (*)(uint32_t, char*, uint32_t);
struct D2ClientMessageNode {
    uintptr_t next;
    uintptr_t previous;
    D2ClientMessageSource source;
};
static_assert(offsetof(D2ClientMessageNode, source) == 2 * sizeof(uintptr_t));
static_assert(sizeof(D2ClientMessageNode) == 3 * sizeof(uintptr_t));
// Production equivalents of retail 0x1233F8, 0x11C56C and 0x11B984. The list's
// base/next/back fields are native addresses; init remains a retail 32-bit scalar.
extern uintptr_t d2_client_message_sources[3];
extern uint32_t d2_client_message_sources_initialized;
extern std::recursive_mutex d2_client_message_sources_mutex;
extern "C" void d2_client_ded0();
extern "C" void d2_client_df00(D2ClientMessageSource);
extern "C" void d2_client_dd20();
// Consumer used by the formatter: newest source first, stop at the first success.
// It exposes the production list directly and never maintains a shadow registry.
int32_t d2_client_try_message_sources(uint32_t, char*, uint32_t);
extern "C" void d2_client_dca0(uint32_t, char*, uint32_t);
