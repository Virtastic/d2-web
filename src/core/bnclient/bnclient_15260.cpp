// 1.13c BNClient+0x15260 BNGatewayAccess::UpdateGatewaysFromIni(this, ini) (stdcall, ret 8), own-unit form.
// Parse a gateways ini ([Server List Version] VER, [Server Gateways]
// 1=,2=,... and a [<name>] section per gateway with ZONE/ENU) into one NUL-separated buffer
// "<ver>\0" "00\0" then (name, zone, display) per gateway and a closing NUL. A list no newer than this+0x18 is left
// alone. The newer list resets the selection (+0x04 = 0, SaveAndUnload, +0x1C = 0, +0x04 = 1, +0x18 = ver), takes
// the GMT of the current gateway (or the system zone when no buffer is loaded) BEFORE the unload, then installs the
// buffer (+0x10 ptr, +0x14 bytes), picks the closest zone, saves and reloads.
// Every callee goes through the seam so the FN probe can hold retail's calls one for one. The fatal sites
// (0x236 no version section, 0x23B no VER, 0x2A5 buffer overrun, 0x2A7 unterminated buffer) end in _exit, which
// does not return: the unit returns after it.
// The 1.13c body differs from the shared bn_gateway_update_from_ini in one place only: with no ENU key retail copies
// the display name from the name it already copied into the buffer (SStrLen-free inline strlen of that copy),
// not from the ini text; the bytes are the same.
#include <cstdint>
#include <cstring>

struct Bn15260Env {
    uint32_t (*findSection)(uint32_t self, uint32_t data, uint32_t name);   // 0x14E10 thiscall
    uint32_t (*findKey)(uint32_t self, uint32_t data, uint32_t key);        // 0x14D70 thiscall
    uint32_t (*skipToEol)(uint32_t self, uint32_t p, uint32_t end);         // 0x14AD0 thiscall
    uint32_t (*strtoul)(uint32_t s, uint32_t endOut, int32_t base);        // 0x18AF cdecl
    void (*log)(uint32_t fmt, uint32_t value);                              // 0x17420 cdecl
    int32_t (*gmt)(uint32_t self, uint32_t index);                         // 0x14F60 stdcall
    int32_t (*systemTimeZone)(uint32_t self);                              // 0x14D20 thiscall
    void (*saveAndUnload)(uint32_t self);                                   // 0x15030 stdcall
    uint32_t (*alloc)(uint32_t size, uint32_t file, uint32_t line, uint32_t flags);   // Storm #401, thunk 0x8018
    void (*sprintfD)(uint32_t buf, uint32_t fmt, uint32_t value);           // _sprintf 0x159A, "%d"
    uint32_t (*sstrLen)(uint32_t s);                                        // Storm #506, thunk 0x8042
    uint32_t (*sstrCopy)(uint32_t dst, uint32_t src, uint32_t max);         // Storm #501, thunk 0x804E
    void (*pickClosestZone)(uint32_t self, int32_t gmt);                   // 0x15130 thiscall
    void (*load)(uint32_t self);                                            // 0x155D0 stdcall
    uint32_t (*callerRet)(uint32_t line);                                   // Fog #10265, thunk 0x80E4
    void (*halt)(uint32_t msg, uint32_t ret);                               // Fog #10024, thunk 0x80DE
    void (*exit)(int32_t code);                                             // _exit 0x1569
    // module strings (1.13c RVAs): 0x1A8D4 "Server List Version", 0x1A8D0 "VER", 0x1A894 "Server Gateways",
    // 0x1A8A4 "%d", 0x1A88C "ZONE", 0x1A888 "0", 0x1A884 "ENU", 0x1A8A8 the log format, 0x1A93C the source file,
    // 0x1A813 "" (the halt message)
    uint32_t sListVersion, sVer, sGateways, sPctD, sZone, sZero, sEnu, sLogFmt, sFile, sEmpty;
};

namespace {
inline uint32_t rd32(uint32_t a) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
inline void wr32(uint32_t a, uint32_t v) { std::memcpy((void*)(uintptr_t)a, &v, 4); }
inline uint8_t rd8(uint32_t a) { return *(const uint8_t*)(uintptr_t)a; }
inline void wr8(uint32_t a, uint8_t v) { *(uint8_t*)(uintptr_t)a = v; }
inline uint32_t cstrlen(uint32_t s) { uint32_t n = 0; while (rd8(s + n)) ++n; return n; }
void fatal(const Bn15260Env& e, uint32_t line) {
    e.halt(e.sEmpty, e.callerRet(line));
    e.exit(-1);
}
}  // namespace

// 1.13c BNClient+0x15260
// Returns retail's EAX at the ret: the stored version +0x18 on the no-newer path (it is loaded into EAX for the cmp; jbe), else 0.
extern "C" uint32_t bnclient_15260_UpdateGatewaysFromIni(uint32_t self, uint32_t data, const Bn15260Env& e) {
    const uint32_t dataEnd = data + cstrlen(data);
    uint32_t p = e.findSection(self, data, e.sListVersion);
    if (p == 0) { fatal(e, 0x236); return 0; }
    p = e.findKey(self, p, e.sVer);
    if (p == 0) { fatal(e, 0x23B); return 0; }
    uint32_t endp = 0;
    const uint32_t ver = e.strtoul(p, (uint32_t)(uintptr_t)&endp, 10);
    if (ver <= rd32(self + 0x18)) return rd32(self + 0x18);   // jbe: unsigned; EAX holds +0x18
    e.log(e.sLogFmt, ver);
    int32_t gmt = rd32(self + 0x10) != 0 ? e.gmt(self, rd32(self + 0x0C)) * 60   // imul 0x3c at the site
                                         : e.systemTimeZone(self);
    wr32(self + 0x04, 0);
    e.saveAndUnload(self);
    wr32(self + 0x1C, 0);
    wr32(self + 0x04, 1);
    wr32(self + 0x18, ver);

    const uint32_t allocation = (dataEnd - data) * 2;
    const uint32_t buf = e.alloc(allocation, e.sFile, 0x256, 0);
    e.sprintfD(buf, e.sPctD, ver);
    const uint32_t digits = e.sstrLen(buf);
    wr8(buf + digits + 1, '0');
    wr8(buf + digits + 2, '0');
    wr8(buf + digits + 3, 0);
    uint32_t cur = buf + digits + 4;
    const uint32_t gateways = e.findSection(self, data, e.sGateways);
    char number[16];
    const uint32_t num = (uint32_t)(uintptr_t)number;
    uint32_t index = 1;
    e.sprintfD(num, e.sPctD, index);
    uint32_t key = e.findKey(self, gateways, num);
    while (key != 0) {
        uint32_t len = e.skipToEol(self, key, dataEnd) - key;
        e.sstrCopy(cur, key, len + 1);
        const uint32_t name = cur;
        const uint32_t sect = e.findSection(self, data, name);
        if (sect != 0) {
            const uint32_t zoneDst = cur + len + 1;
            const uint32_t zone = e.findKey(self, sect, e.sZone);
            if (zone == 0) {
                len = 1;
                e.sstrCopy(zoneDst, e.sZero, 2);
            } else {
                len = e.skipToEol(self, zone, dataEnd) - zone;
                e.sstrCopy(zoneDst, zone, len + 1);
            }
            cur = zoneDst + len + 1;
            const uint32_t enu = e.findKey(self, sect, e.sEnu);
            uint32_t src;
            if (enu == 0) {
                src = name;
                len = cstrlen(name);
            } else {
                src = enu;
                len = e.skipToEol(self, enu, dataEnd) - enu;
            }
            e.sstrCopy(cur, src, len + 1);
            cur = cur + len + 1;
        }
        ++index;
        e.sprintfD(num, e.sPctD, index);
        key = e.findKey(self, gateways, num);
    }
    wr8(cur, 0);
    const int32_t size = (int32_t)(cur + 1 - buf);
    if (!(size < (int32_t)allocation)) { fatal(e, 0x2A5); return 0; }   // jl: signed
    if (rd8(buf + size - 1) != 0 || rd8(buf + size - 2) != 0) { fatal(e, 0x2A7); return 0; }
    wr32(self + 0x10, buf);
    wr32(self + 0x14, (uint32_t)size);
    e.pickClosestZone(self, gmt);
    e.saveAndUnload(self);
    e.load(self);
    return 0;
}
