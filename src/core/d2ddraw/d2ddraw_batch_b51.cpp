// src/core/d2ddraw/d2ddraw_batch_b51.cpp -- UNMAPPED 1.13c D2DDraw.dll rows reimplemented from the cached
// decompiles (alpha lane, no spec/vector yet). Globals are named at their retail RVA (base 0x6F8C0000).
// Skipped as not faithfully portable: 0x51D0 (LCMapStringA CRT body, stack-shuffled), 0x560C (__SEH
// security-failure box), 0x6E20 (register-passed ordinal clip), 0x6FF0 (Bink), 0x82F0/0x85A0 (COM vtable
// calls on DirectDraw), 0x9644 (CRT fn-pointer table of code labels).
#include <cstdint>

static int32_t  g_width;    // +0x101C8 DAT_6f8d01c8: surface width / row pitch
static int32_t  g_height;   // +0x101D8 DAT_6f8d01d8
static uint8_t* g_surface;  // +0x0F8A8 DAT_6f8cf8a8: 8-bit render buffer

// 1.13c D2DDraw+0x06010 (FillRect8): rect {l,t,r,b}; clip to the surface, fill width r-l, height b-t
// (exclusive, as retail) with colour. Bad rect order is fatal in retail (caller seam).
extern "C" void (*d2ddraw_fatal)(uint32_t code);
extern "C"
void d2ddraw_fill_rect(int32_t* r, uint8_t color) {
    int32_t l = r[0], t = r[1], rt = r[2], b = r[3];
    if (rt < l) { d2ddraw_fatal(0x165); return; }
    if (t > b) { d2ddraw_fatal(0x166); return; }
    if (rt < 0 || l >= g_width || b < 0 || t >= g_height) return;
    if (l < 0) r[0] = 0;
    if (rt >= g_width) r[2] = g_width - 1;
    if (t < 0) r[1] = 0;
    if (b >= g_height) r[3] = g_height - 1;
    uint32_t w = (uint32_t)(r[2] - r[0]);
    int32_t h = r[3] - r[1];
    if (!w || !h) return;
    uint8_t* row = g_surface + r[1] * g_width + r[0];
    do { for (uint32_t i = 0; i < w; ++i) row[i] = color; row += g_width; } while (--h);
}

// 1.13c D2DDraw+0x06130 (DrawLine8): 16.16 stepping along p1 -> p0 from p0's start point; steps =
// ceil(sqrt(dx^2+dy^2)); off-surface start points are skipped until on-surface.
extern "C"
void d2ddraw_draw_line(const int32_t* p1, const int32_t* p0, uint8_t color) {
    int32_t y = p0[1], x = p0[0];
    int32_t dx = p1[0] - x, dy = p1[1] - y;
    if (!dx && !dy) return;
    int32_t d2 = dy * dy + dx * dx, n = 0;
    if (d2 > 0) do { ++n; } while (n * n < d2);
    int32_t sx = (dx * 0x10000) / n, sy = (dy * 0x10000) / n;
    int32_t fx = 0, fy = 0, i = 0;
    while (y < 0 || y >= g_height || x < 0 || x >= g_width) {
        fx += sx;
        if (fx > 0xffff || fx < -0xffff) { int32_t s = (fx + ((fx >> 31) & 0xffff)) >> 16; x += s; fx -= s * 0x10000; }
        fy += sy;
        if (fy > 0xffff || fy < -0xffff) { int32_t s = (fy + ((fy >> 31) & 0xffff)) >> 16; y += s; fy -= s * 0x10000; }
        if (n <= i) return;
        ++i;
    }
    uint8_t* p = g_surface + g_width * y + x;
    *p = color;
    if (++i >= n) return;
    for (;;) {
        fx += sx;
        if (fx > 0xffff || fx < -0xffff) { int32_t s = (fx + ((fx >> 31) & 0xffff)) >> 16; x += s; p += s; fx -= s * 0x10000; }
        if (x < 0 || x >= g_width) return;
        fy += sy;
        if (fy > 0xffff || fy < -0xffff) { int32_t s = (fy + ((fy >> 31) & 0xffff)) >> 16; y += s; p += g_width * s; fy -= s * 0x10000; }
        if (y < 0 || y >= g_height) return;
        *p = color;
        if (n <= ++i) return;
    }
}

// 1.13c D2DDraw+0x05C5E (___isctype_mt): ctype lookup. pctype = locale +0x48 table, codepage +4,
// lcid +0x14. Chars -1..255 index the table; others go through __crtGetStringTypeA (0x4F88) via seam.
struct D2DDrawLocale { uint8_t pad0[4]; uint32_t cp; uint8_t pad1[0x0c]; uint32_t lcid; uint8_t pad2[0x30]; const uint16_t* pctype; };
extern "C" int32_t (*d2ddraw_gst)(uint32_t info, const char* s, int32_t cch, uint16_t* out, uint32_t cp, uint32_t lcid);
extern "C"
uint32_t d2ddraw_isctype_mt(const D2DDrawLocale* loc, int32_t c, uint32_t mask) {
    uint16_t t;
    if ((uint32_t)(c + 1) < 0x101) t = loc->pctype[c];
    else {
        const uint8_t* hi = reinterpret_cast<const uint8_t*>(loc->pctype);
        char buf[3]; int32_t n;
        if (hi[1 + ((c >> 8) & 0xff) * 2] & 0x80) { buf[0] = (char)(c >> 8); buf[1] = (char)c; n = 2; }
        else { buf[0] = (char)c; buf[1] = 0; n = 1; }
        buf[2] = 0;
        if (!d2ddraw_gst(1, buf, n, &t, loc->cp, loc->lcid)) return 0;
    }
    return t & mask;
}

// 1.13c D2DDraw+0x07690 (DDraw error logger): name an HRESULT and log it; benign codes
// 0x887601ae and 0x887601c2 (surface lost/busy) return silently. Log is Fog ordinal 10029 (seam).
extern "C" void (*d2ddraw_log)(const char* fmt, const char* what, const char* name, uint32_t hr);
extern "C"
void d2ddraw_log_error(uint32_t hr, const char* what) {
    static const struct { uint32_t hr; const char* name; } tbl[] = {
    {0x88760241u, "DDERR_OVERLAYNOTVISIBLE"},
    {0x887601ccu, "DDERR_SURFACENOTATTACHED"},
    {0x887601d6u, "DDERR_TOOBIGHEIGHT"},
    {0x887601e0u, "DDERR_TOOBIGSIZE"},
    {0x887601eau, "DDERR_TOOBIGWIDTH"},
    {0x887601feu, "DDERR_UNSUPPORTEDFORMAT"},
    {0x88760208u, "DDERR_UNSUPPORTEDMASK"},
    {0x88760219u, "DDERR_VERTICALBLANKINPROGRESS"},
    {0x8876021cu, "DDERR_WASSTILLDRAWING"},
    {0x88760230u, "DDERR_XALIGN"},
    {0x88760231u, "DDERR_INVALIDDIRECTDRAWGUID"},
    {0x88760232u, "DDERR_DIRECTDRAWALREADYCREATED"},
    {0x88760233u, "DDERR_NODIRECTDRAWHW"},
    {0x88760234u, "DDERR_PRIMARYSURFACEALREADYEXISTS"},
    {0x88760235u, "DDERR_NOEMULATION"},
    {0x88760236u, "DDERR_REGIONTOOSMALL"},
    {0x88760237u, "DDERR_CLIPPERISUSINGHWND"},
    {0x88760238u, "DDERR_NOCLIPPERATTACHED"},
    {0x88760239u, "DDERR_NOHWND"},
    {0x8876023au, "DDERR_HWNDSUBCLASSED"},
    {0x8876023bu, "DDERR_HWNDALREADYSET"},
    {0x8876023cu, "DDERR_NOPALETTEATTACHED"},
    {0x8876023du, "DDERR_NOPALETTEHW"},
    {0x8876023eu, "DDERR_BLTFASTCANTCLIP"},
    {0x8876023fu, "DDERR_NOBLTHW"},
    {0x88760240u, "DDERR_NODDROPSHW"},
    {0x88760276u, "DDERR_NONONLOCALVIDMEM"},
    {0x88760242u, "DDERR_NOOVERLAYDEST"},
    {0x88760243u, "DDERR_INVALIDPOSITION"},
    {0x88760244u, "DDERR_NOTAOVERLAYSURFACE"},
    {0x88760245u, "DDERR_EXCLUSIVEMODEALREADYSET"},
    {0x88760246u, "DDERR_NOTFLIPPABLE"},
    {0x88760247u, "DDERR_CANTDUPLICATE"},
    {0x88760248u, "DDERR_NOTLOCKED"},
    {0x88760249u, "DDERR_CANTCREATEDC"},
    {0x8876024au, "DDERR_NODC"},
    {0x8876024bu, "DDERR_WRONGMODE"},
    {0x8876024cu, "DDERR_IMPLICITLYCREATED"},
    {0x8876024du, "DDERR_NOTPALETTIZED"},
    {0x8876024eu, "DDERR_UNSUPPORTEDMODE"},
    {0x8876024fu, "DDERR_NOMIPMAPHW"},
    {0x88760250u, "DDERR_INVALIDSURFACETYPE"},
    {0x88760258u, "DDERR_NOOPTIMIZEHW"},
    {0x88760259u, "DDERR_NOTLOADED"},
    {0x8876026cu, "DDERR_DCALREADYCREATED"},
    {0x887602b2u, "DDERR_MOREDATA"},
    {0x88760280u, "DDERR_CANTPAGELOCK"},
    {0x88760294u, "DDERR_CANTPAGEUNLOCK"},
    {0x887602a8u, "DDERR_NOTPAGELOCKED"},
    {0x887602b7u, "DDERR_VIDEONOTACTIVE"},
    {0x887602bbu, "DDERR_DEVICEDOESNTOWNSURFACE"},
    {0x887601b8u, "DDERR_SURFACEISOBSCURED"},
    {0x887600e1u, "DDERR_NOEXCLUSIVEMODE"},
    {0x887600e6u, "DDERR_NOFLIPHW"},
    {0x887600f0u, "DDERR_NOGDI"},
    {0x887600fau, "DDERR_NOMIRRORHW"},
    {0x887600ffu, "DDERR_NOTFOUND"},
    {0x88760104u, "DDERR_NOOVERLAYHW"},
    {0x88760118u, "DDERR_NORASTEROPHW"},
    {0x88760122u, "DDERR_NOROTATIONHW"},
    {0x88760136u, "DDERR_NOSTRETCHHW"},
    {0x8876013cu, "DDERR_NOT4BITCOLOR"},
    {0x8876013du, "DDERR_NOT4BITCOLORINDEX"},
    {0x88760140u, "DDERR_NOT8BITCOLOR"},
    {0x8876014au, "DDERR_NOTEXTUREHW"},
    {0x8876014fu, "DDERR_NOVSYNCHW"},
    {0x88760154u, "DDERR_NOZBUFFERHW"},
    {0x8876015eu, "DDERR_NOZOVERLAYHW"},
    {0x88760168u, "DDERR_OUTOFCAPS"},
    {0x8876017cu, "DDERR_OUTOFVIDEOMEMORY"},
    {0x8876017eu, "DDERR_OVERLAYCANTCLIP"},
    {0x88760180u, "DDERR_OVERLAYCOLORKEYONLYONEACTIVE"},
    {0x88760183u, "DDERR_PALETTEBUSY"},
    {0x88760190u, "DDERR_COLORKEYNOTSET"},
    {0x8876019au, "DDERR_SURFACEALREADYATTACHED"},
    {0x887601a4u, "DDERR_SURFACEALREADYDEPENDENT"},
    {0x887601aeu, "DDERR_SURFACEBUSY"},
    {0x887601b3u, "DDERR_CANTLOCKSURFACE"},
    {0x887600deu, "DDERR_NODIRECTDRAWSUPPORT"},
    {0x88760078u, "DDERR_INVALIDMODE"},
    {0x88760082u, "DDERR_INVALIDOBJECT"},
    {0x88760091u, "DDERR_INVALIDPIXELFORMAT"},
    {0x88760096u, "DDERR_INVALIDRECT"},
    {0x887600a0u, "DDERR_LOCKEDSURFACES"},
    {0x887600aau, "DDERR_NO3D"},
    {0x887600b4u, "DDERR_NOALPHAHW"},
    {0x887600cdu, "DDERR_NOCLIPLIST"},
    {0x887600d2u, "DDERR_NOCOLORCONVHW"},
    {0x887600d4u, "DDERR_NOCOOPERATIVELEVELSET"},
    {0x887600d7u, "DDERR_NOCOLORKEY"},
    {0x887600dcu, "DDERR_NOCOLORKEYHW"},
    {0x8876006eu, "DDERR_INVALIDCLIPLIST"},
    {0x8876000au, "DDERR_CANNOTATTACHSURFACE"},
    {0x8007000eu, "DDERR_OUTOFMEMORY"},
    {0x80004001u, "DDERR_UNSUPPORTED"},
    {0x80004005u, "DDERR_GENERIC"},
    {0x800401f0u, "DDERR_NOTINITIALIZED"},
    {0x80070057u, "DDERR_INVALIDPARAMS"},
    {0x88760005u, "DDERR_ALREADYINITIALIZED"},
    {0x88760014u, "DDERR_CANNOTDETACHSURFACE"},
    {0x88760028u, "DDERR_CURRENTLYNOTAVAIL"},
    {0x88760037u, "DDERR_EXCEPTION"},
    {0x8876005au, "DDERR_HEIGHTALIGN"},
    {0x8876005fu, "DDERR_INCOMPATIBLEPRIMARY"},
    {0x88760064u, "DDERR_INVALIDCAPS"},
    };
    if (hr == 0x887601aeu || hr == 0x887601c2u) return;
    const char* name = "< unknown >";
    for (const auto& e : tbl) if (e.hr == hr) { name = e.name; break; }
    d2ddraw_log("***** DirectDraw error: %s - %s (0x%x)", what, name, hr);
}
