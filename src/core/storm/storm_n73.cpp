// Storm SEH unwind funnels and sub_38F60; host seams are function-pointer args.
#include <cstdint>
typedef void (*Storm_Fn0)();
typedef void (*Storm_Fn1)(void*);
typedef int32_t (*Storm_Free)(void*, const char*, int32_t);

// 1.13c STORM+0x423E0
extern "C" void Storm_Unwind_423E0(Storm_Fn0 dtor_4C20) { dtor_4C20(); }

// 1.13c STORM+0x42360
extern "C" void Storm_Unwind_42360(Storm_Fn0 dtor_1E90) { dtor_1E90(); }

// 1.13c STORM+0x42340
extern "C" void Storm_Unwind_42340(Storm_Fn0 dtor_1F10) { dtor_1F10(); }

// 1.13c STORM+0x42320
extern "C" void Storm_Unwind_42320(Storm_Fn0 dtor_1FA0) { dtor_1FA0(); }

// 1.13c STORM+0x422D0
extern "C" void Storm_Unwind_422D0(Storm_Fn1 dtor_1A70, uint8_t* ebp) { dtor_1A70(*(void**)(ebp + 4)); }

// 1.13c STORM+0x422AD
extern "C" void Storm_Unwind_422AD(Storm_Fn0 dtor_2290) { dtor_2290(); }

// 1.13c STORM+0x42286
extern "C" void Storm_Unwind_42286(Storm_Fn0 dtor_4C20) { dtor_4C20(); }

// 1.13c STORM+0x42260
extern "C" void Storm_Unwind_42260(Storm_Fn0 dtor_4C20) { dtor_4C20(); }

// 1.13c STORM+0x42235
extern "C" void Storm_Unwind_42235(Storm_Fn0 dtor_4C20) { dtor_4C20(); }

// 1.13c STORM+0x42205
extern "C" void Storm_Unwind_42205(Storm_Fn0 dtor_2740) { dtor_2740(); }

// 1.13c STORM+0x42140
extern "C" void Storm_Unwind_42140(Storm_Fn1 dtor_1A70, uint8_t* ebp) { dtor_1A70(*(void**)(ebp + 4)); }

// 1.13c STORM+0x38F60
extern "C" void Storm_Sub38F60(uint8_t* self, Storm_Fn1 sub4D60, Storm_Free smemfree) {
    int32_t* p;
    while (p = *(int32_t**)(self + 0x14), 0 < (int32_t)(intptr_t)p) {
        sub4D60(p);
        smemfree(p, "AUINST", -2);
    }
}
