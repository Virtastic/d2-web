// 1.13c Fog+0x16930 (ordinal 10015, ResolveHostIPAddress): ECX = host string, plain ret. The port
// (fog_sub_16930 in src/core/fog/fog_batch_105d0.cpp) takes the seam through FogNetApi; this probe
// fills it with the hook-recording callees. Two descriptors drive it: a passthrough-and-failure one
// and a resolved one (the host entry chain). Blob st (r[16]) scripts the three words the hooks
// return: gethostbyname (0 = miss, non-zero = hit), WSAGetLastError, WSAStartup.
#include <emscripten.h>
#include <cstdint>
#include <cstdio>
#include "hookenv.h"

namespace {

struct FogNetApi {
    void* (*gethostbyname_)(const char* name);
    const char* (*inet_ntoa_)(uint32_t addr);
    int32_t (*wsastartup_)(uint32_t version, void* data);
    int32_t (*wsagetlasterror_)(void);
    void (*wsaunhookblockinghook_)(void);
};

HookEnv* gHookEnv = nullptr;
uint32_t gAddr = 0;              // the four address bytes of the test host, read as one dword
uint32_t gList[2] = { 0, 0 };    // h_addr_list: entry 0 points at gAddr
uint32_t gHe[4] = { 0, 0, 0, 0 };  // hostent: +0x0C is h_addr_list

uint32_t wordPtr(const void* p) { return static_cast<uint32_t>(reinterpret_cast<uintptr_t>(p)); }

void* probeGetHostByName(const char* name) {
    const uint32_t hit = gHookEnv->h(0xCD7C, 0, 0, 0, 0, 0, wordPtr(name));
    if (hit == 0) return nullptr;
    gAddr = 0x0302010Au;
    gList[0] = wordPtr(&gAddr);
    gHe[3] = wordPtr(gList);
    return gHe;
}

int32_t probeWsaGetLastError(void) { return static_cast<int32_t>(gHookEnv->h(0xCD3A)); }

int32_t probeWsaStartup(uint32_t version, void* data) {
    return static_cast<int32_t>(gHookEnv->h(0xCDA0, 0, 0, 0, 0, 0, version, wordPtr(data)));
}

void probeWsaUnhook(void) { gHookEnv->h(0xCD8E); }

const char* probeInetNtoa(uint32_t addr) {
    gHookEnv->h(0xCDA6, 0, 0, 0, 0, 0, addr);
    static char text[16];
    std::snprintf(text, sizeof text, "%u.%u.%u.%u", addr & 0xFFu, (addr >> 8) & 0xFFu, (addr >> 16) & 0xFFu, addr >> 24);
    return text;
}

}  // namespace

extern "C" char* fog_sub_16930(const void* api, char* host);

extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_fog_16930(uint32_t* r) {
    const uint32_t* st = reinterpret_cast<const uint32_t*>(static_cast<uintptr_t>(r[16]));
    const uint32_t sc[8] = { 0xCD7Cu, st[0], 0xCD3Au, st[1], 0xCDA0u, st[2], 0u, 0u };
    HookEnv e(reinterpret_cast<uint32_t*>(static_cast<uintptr_t>(r[14])), sc);
    gHookEnv = &e;
    FogNetApi api = {};
    api.gethostbyname_ = probeGetHostByName;
    api.inet_ntoa_ = probeInetNtoa;
    api.wsastartup_ = probeWsaStartup;
    api.wsagetlasterror_ = probeWsaGetLastError;
    api.wsaunhookblockinghook_ = probeWsaUnhook;
    r[4] = wordPtr(fog_sub_16930(&api, reinterpret_cast<char*>(static_cast<uintptr_t>(r[0]))));
    gHookEnv = nullptr;
}
