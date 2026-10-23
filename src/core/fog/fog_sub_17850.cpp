// Fog+0x17850: open a UDP-ish listen socket on `port` (winsock ordinals injected; non-framable, no wasm socket).
#include <cstdint>
struct FogSockApi {
    int (*socket_)(int, int, int); int (*setsockopt_)(int, int, int, const void*, int);
    int (*ioctlsocket_)(int, long, uint32_t*); uint16_t (*htons_)(uint16_t);
    int (*bind_)(int, const void*, int); int (*listen_)(int, int); int (*closesocket_)(int);
};
// 1.13c Fog+0x17850
extern "C" int fog_sub_17850(uint8_t* ctx, uint16_t port, const FogSockApi* w) {
    *(uint32_t*)(ctx + 0x884) = 1;
    int s = w->socket_(2, 1, 6);  // 2,1,6 per stack setup (AF_INET, stream, tcp)
    if (s == -1) return 0;
    *(int*)(ctx + 0xb90) = s;
    uint32_t one = 1;
    w->setsockopt_(s, 6, 1, &one, 4);       // TCP_NODELAY
    w->setsockopt_(s, 0xffff, 8, &one, 4);  // SO_KEEPALIVE
    w->setsockopt_(s, 0xffff, 4, &one, 4);  // SO_REUSEADDR
    w->ioctlsocket_(s, (long)0x8004667e, &one);  // FIONBIO
    *(uint16_t*)(ctx + 0xba0) = port;
    uint16_t sa[8] = {2, w->htons_(port), 0, 0, 0, 0, 0, 0};
    if (w->bind_(*(int*)(ctx + 0xb90), sa, 16) == -1) { w->closesocket_(s); return 0; }
    if (w->listen_(s, 0x19) == -1) w->closesocket_(s);
    // retail then CreateEvent -> ctx+0xbd4 and CreateThread(0x17390,ctx) -> ctx+0xbd8; platform-only, omitted
    return 1;
}
