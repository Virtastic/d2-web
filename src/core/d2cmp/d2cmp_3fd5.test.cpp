// Host test: g++ -o /tmp/t src/core/d2cmp/d2cmp_3fd5.test.cpp && /tmp/t
#include "d2cmp_3fd5.cpp"
#include <cassert>
static int fr, sets; static void* freed; static void* val = (void*)8;
static void* g(uint32_t i) { assert(i == 5); return val; }
static void s(uint32_t i, void* v) { assert(i == 5 && !v); sets++; }
static void f(void* p) { fr++; freed = p; }
int main() {
    D2cmpFreeptdEnv e{-1, g, s, f};
    d2cmp_3fd5(nullptr, &e); assert(!fr && !sets);
    e.tls_index = 5;
    d2cmp_3fd5(nullptr, &e); assert(fr == 1 && freed == val && sets == 1);
    d2cmp_3fd5((void*)3, &e); assert(freed == (void*)3 && sets == 2);
}
