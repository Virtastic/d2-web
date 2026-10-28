#pragma once
#include <cstddef>
#include <cstdint>
// Serialized retail representation is never used as a native pointer container.
struct D2ClientRetailList { uint32_t base, next, encodedPrevious; };
static_assert(sizeof(D2ClientRetailList) == 12);
static_assert(offsetof(D2ClientRetailList, encodedPrevious) == 8);
using D2ClientListAlloc = void* (*)(uint32_t, const char*, uint32_t, uint32_t);
extern "C" {
void d2_client_2400(uintptr_t, uintptr_t, uint32_t, uintptr_t);
void d2_client_2f00(uintptr_t*);
void d2_fn_d2client_2ff0(uintptr_t*);
void d2_fn_d2client_30a0(uintptr_t*);
void d2_fn_d2client_2ec0(void*);
void* d2_client_2f70(uintptr_t, int32_t, uint32_t, uint32_t, D2ClientListAlloc);
}
