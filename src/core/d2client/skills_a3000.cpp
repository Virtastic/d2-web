// D2Client rva 0xA3000: skill-table lookup then sub_A2C90. Callees injected.
#include <cstdint>

struct D2ClientA3000Callees {
    uint32_t (*ord10511)();                                        // handle; 0 -> return
    uint32_t (*ord10601)(uint32_t h, const char* file, uint32_t line);
    uint32_t (*ord10306)();
    void (*subA2C90)(uint32_t* edi, uint32_t a, uint32_t b, uint32_t zero);
};

// 1.13c D2Client+0xA3000 (edi = unaffected EDI passed through to A2C90)
void d2_client_a3000(const D2ClientA3000Callees& c, uint32_t* edi) {
    uint32_t h = c.ord10511();
    if (!h) return;
    uint32_t r = c.ord10601(h, "..\\Source\\D2Client\\SKILLS\\Skills.cpp", 0x697);
    c.subA2C90(edi, r, c.ord10306(), 0);
}
