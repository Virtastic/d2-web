// 1.13c Storm+0x21220: SDlg window-class registration. Sets the "classes needed" flag (Storm+0x553B8), registers the
// SDlgDialog class, then the SDlgStatic class (a copy of the system Static class with the CS_GLOBALCLASS bit cleared), and
// reports whether both registrations produced an atom. The Win32 calls are the env's; RegisterClassA's ATOM is 16-bit.
#include <cstdint>
struct Storm21220Env {
    uint32_t* classesNeeded;                    // Storm+0x553B8
    uint32_t (*registerClass)(uint32_t which);  // 0: "SDlgDialog", 1: "SDlgStatic"; the ATOM in the low 16 bits
    void (*getStaticClassInfo)();               // GetClassInfoA(module, "Static", &wc): result unused by retail
};
extern "C" int32_t Storm_Sub21220(const Storm21220Env* e) {   // 1.13c Storm+0x21220
    *e->classesNeeded = 1;
    if ((uint16_t)e->registerClass(0) == 0) return 0;
    e->getStaticClassInfo();
    return (uint16_t)e->registerClass(1) != 0;
}
