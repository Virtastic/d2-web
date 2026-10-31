// 1.13c D2DDraw+0x08f80 -- free the video frame buffer and close a Smacker handle. Decompile-derived.
#include <cstdint>

struct D2DDrawVideo { uint32_t handle; uint8_t kind; };  // kind 's' = Smacker

struct D2DDrawVideoHost {
    uint32_t* frameBuf;                                            // +0xF780
    void      (*fogFree)(uint32_t p, const char* file, uint32_t line, uint32_t pool);  // Fog 10043
    const char* file;                                              // 0x6F8CC25C
    void      (*smackClose)(uint32_t handle);                      // SmackClose@4
};

// 1.13c D2DDraw+0x08f80
// A live frame buffer is freed (Fog 10043, line 0xF0, pool 0) and cleared; a Smacker handle (kind 's',
// handle non-zero) is closed and zeroed. Other kinds are left alone.
extern "C" void d2ddraw_video_close(const D2DDrawVideoHost* h, D2DDrawVideo* v) {
    if (*h->frameBuf) { h->fogFree(*h->frameBuf, h->file, 0xF0, 0); *h->frameBuf = 0; }
    if (v->kind == 's' && v->handle) { h->smackClose(v->handle); v->handle = 0; }
}
