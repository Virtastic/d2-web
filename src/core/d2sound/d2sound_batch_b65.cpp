#include <cstddef>
#include <cstdint>
#include <initializer_list>

// Property-set object (retail DAT_6f9c64b8): vtable slot +0x10 is Get/Set(guid, id, inst, instLen, data, len).
struct B65PropSet;
struct B65PropSetVtbl { void* slots[4]; int (*Set)(B65PropSet*, const void* guid, ...); };
struct B65PropSet { const B65PropSetVtbl* vt; };
using B65PropFn = int (*)(B65PropSet*, const void*, uint32_t, uint32_t, uint32_t, const void*, uint32_t);
static inline int b65_prop(B65PropSet* o, const void* g, uint32_t id, const void* data)
{ return ((B65PropFn)o->vt->slots[4])(o, g, id, 0, 0, data, 4); }
static inline int b65_open(B65PropSet* o, const void* g)
{ return ((int (*)(B65PropSet*, const void*))o->vt->slots[4])(o, g); }

static B65PropSet* g_b65_prop;      // DAT_6f9c64b8
static int g_b65_enabled;           // DAT_6f9c6594
static int g_b65_mode;              // DAT_6f9c6598 (1 = 3D listener, 2 = EAX-style reverb)
static uint32_t g_b65_flag;         // DAT_6f9c659c
static uint8_t g_b65_guidA[16], g_b65_guidB[16]; // DAT_6f9c01d8 / DAT_6f9c01f8
static int32_t (*g_b65_fn87D0)(float); // FUN_6f9b87d0 (result unused)

// 1.13c D2sound+0x8FD0  (__fastcall: param_1 in ECX)
void D2sound_b65_8FD0(const int32_t* p)
{
    if (!g_b65_enabled || !g_b65_prop) return;
    B65PropSet* o = g_b65_prop;
    uint32_t hdr[3] = {1, 0, 0};
    if (g_b65_mode == 1) {
        g_b65_fn87D0((float)p[3]);
        b65_open(o, g_b65_guidA);
        b65_prop(o, g_b65_guidA, 2, hdr);
        b65_prop(o, g_b65_guidA, 3, p + 5);
    }
    if (g_b65_mode == 2) {
        hdr[0] = 0xb;
        b65_open(o, g_b65_guidB);
        b65_prop(o, g_b65_guidB, 0xf, hdr);
        const struct { uint32_t id, off; } m[] = {{0xb,0},{0xc,1},{0xd,2},{2,3},{3,4},{5,5},{6,6},{7,7},{8,8},{9,9},{10,10},{4,11},{0xe,12}};
        for (auto& e : m) b65_prop(o, g_b65_guidB, e.id, p + e.off);
    }
    g_b65_flag = (p[3] == -10000);
}

// 1.13c D2sound+0x7CD0  (EAX register input becomes explicit leading param)
static uint32_t g_b65_c8, g_b65_h580;
static void (*g_b65_ord260)(uint32_t);
static uint32_t (*g_b65_fn7740)(int, uint32_t);
static uint32_t g_b65_arg658c;
void D2sound_b65_7CD0(uint32_t eax_in)
{
    g_b65_c8 = eax_in;
    g_b65_ord260(g_b65_arg658c);
    g_b65_h580 = g_b65_fn7740(1, g_b65_c8);
}

// 1.13c D2sound+0x8DB0  (__fastcall: param_1 in ECX)
int D2sound_b65_8DB0(int mode)
{
    if (!g_b65_prop) return 0;
    int32_t v = 0;
    if (mode == 1) {
        if (((B65PropFn)g_b65_prop->vt->slots[4])(g_b65_prop, g_b65_guidA, 2, 0, 0, &v, 4) < 0) return 0;
        g_b65_flag = 1; return 1;
    }
    v = -10000;
    if (((B65PropFn)g_b65_prop->vt->slots[4])(g_b65_prop, g_b65_guidB, 2, 0, 0, &v, 4) < 0) return 0;
    if (((B65PropFn)g_b65_prop->vt->slots[4])(g_b65_prop, g_b65_guidB, 3, 0, 0, &v, 4) < 0) return 0;
    g_b65_flag = 1; return 1;
}
