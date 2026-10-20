#include <new>
#include "../../src/core/d2game/crt_validate_eh3rn_7e6e.cpp"
#include <cassert>
#include <cstring>
#include <sys/mman.h>
static int queries;
static uint32_t protection = 2, type = 0x1000000;
static void* image;
static uintptr_t query(const void*, D2GameMemoryBasicInformation* m) {
    ++queries; m->allocation_base = image; m->protect = protection; m->type = type; return 1;
}
static int32_t exchange(int32_t* p, int32_t v) { auto old = *p; *p = v; return old; }
template<class T> static void put(char* p, T v) { std::memcpy(p, &v, sizeof v); }
int main() {
    D2GameEh3Scope stack_scope{0xffffffffu, 0, 0};
    D2GameEh3Registration rn{nullptr, nullptr, 0, &stack_scope, 0xffffffffu};
    assert(uintptr_t(&stack_scope) > UINT32_MAX);
    D2GameEh3rnCache cache;
    D2GameEh3rnEnv env{uintptr_t(&stack_scope), uintptr_t(&stack_scope)+sizeof stack_scope,
        query, exchange, &cache};
    assert(d2game_validate_eh3rn_7e6e(&rn, &env) == 0); // baseline returned 1 after truncating scope
    auto* mem = static_cast<char*>(mmap(nullptr, 8192, PROT_READ|PROT_WRITE,
        MAP_PRIVATE|MAP_ANONYMOUS, -1, 0));
    assert(mem != MAP_FAILED && uintptr_t(mem) > UINT32_MAX);
    image = mem;
    auto* scope = new(mem+4096) D2GameEh3Scope{0xffffffffu, 0, 0};
    rn.scope = scope; rn.try_level = 0;
    env.stack_limit = 0; env.stack_base = 0;
    assert(d2game_validate_eh3rn_7e6e(&rn, &env) == 1 && queries == 1);
    assert(cache.pages[0] == uintptr_t(scope));
    assert(d2game_validate_eh3rn_7e6e(&rn, &env) == 1 && queries == 1);
    // A low-32-bit alias must miss the cache and query this distinct page.
    cache.pages[0] = uintptr_t(scope) ^ (uintptr_t{1} << 32);
    assert(d2game_validate_eh3rn_7e6e(&rn, &env) == 1 && queries == 2);
    cache.count = 2; cache.pages[1] = uintptr_t(scope);
    cache.pages[0] = uintptr_t(scope) + 4096;
    assert(d2game_validate_eh3rn_7e6e(&rn, &env) == 1);
    assert(cache.pages[0] == uintptr_t(scope));
    scope->enclosing_level = 0;
    assert(d2game_validate_eh3rn_7e6e(&rn, &env) == 0);
    scope->enclosing_level = 0xffffffffu;
    scope->filter = uintptr_t(scope);
    rn.saved_frame = reinterpret_cast<void*>(uintptr_t(&rn)-16);
    env.stack_limit = uintptr_t(&rn)-32;
    assert(d2game_validate_eh3rn_7e6e(&rn, &env) == 1);
    rn.saved_frame = &rn;
    assert(d2game_validate_eh3rn_7e6e(&rn, &env) == 0);
    scope->filter = 0; env.stack_limit = 0; cache = {};
    type = 0; assert(d2game_validate_eh3rn_7e6e(&rn, &env) == UINT32_MAX);
    type = 0x1000000; protection = 4;
    assert(d2game_validate_eh3rn_7e6e(&rn, &env) == UINT32_MAX);
    put<uint16_t>(mem, 0x5a4d); put<int32_t>(mem+0x3c, 0x80);
    put<uint32_t>(mem+0x80, 0x4550); put<uint16_t>(mem+0x98, 0x10b);
    put<uint16_t>(mem+0x86, 1); put<uint16_t>(mem+0x94, 0xe0);
    char* section = mem+0x178;
    put<uint32_t>(section+0xc, 4096); put<uint32_t>(section+8, 4096);
    section[0x27] = char(0x80);
    assert(d2game_validate_eh3rn_7e6e(&rn, &env) == 0);
    section[0x27] = 0;
    assert(d2game_validate_eh3rn_7e6e(&rn, &env) == 1);
    munmap(mem, 8192);
}
