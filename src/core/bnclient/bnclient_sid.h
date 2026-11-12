// BNClient: what the entries of the BNCS receive dispatch table share. Every entry is handler(ECX = payload, EDX = size)
// and returns 1 when the message was taken, 0 when it was refused. A handler reaches the rest of the module through
// BnSidEnv::call: the callee's entry point, its register arguments and its stack words, in the order retail pushes them.
#pragma once
#include <cstdint>
#include <cstring>
#include <initializer_list>

struct BnSidRegs { uintptr_t eax = 0, ecx = 0, edx = 0, ebx = 0; };
struct BnSidEnv {
    uint32_t (*call)(uint32_t entry, const BnSidRegs& regs, const uintptr_t* stack, uint32_t nstack);
    void* (*alloc)(uint32_t size, const char* file, uint32_t line);      // Storm SMemAlloc
    void (*release)(void* p, const char* file, uint32_t line);           // Storm SMemFree
};
inline uint32_t bnSidCall(const BnSidEnv* env, uint32_t entry, const BnSidRegs& regs, std::initializer_list<uintptr_t> stack = {}) {
    return env->call(entry, regs, stack.begin(), (uint32_t)stack.size());
}
inline uint32_t bnSidU32(const uint8_t* payload, uint32_t index) { uint32_t v; memcpy(&v, payload + 4 * index, 4); return v; }

// module state the handlers write
extern "C" {
extern uint32_t g_bn_6ff3eed8[0x5e];   // one answer slot per message id
extern uint32_t g_bn_6ff3e120;         // logon result
extern uint32_t g_bn_6ff3f1f4;         // first error, kept until read
extern uint32_t g_bn_6ff3eed0;         // account result
extern uint32_t g_bn_6ff3f050;         // create-account result
extern uint32_t g_bn_6ff3eb50;         // upgrade in progress
extern uint8_t g_bn_6ff3ecb8[0x40];    // account name being logged on
extern uint32_t g_bn_6ff3f874;         // server token
extern uint32_t g_bn_6ff3f668;         // e-mail registration asked for
extern uint8_t* g_bn_6ff3f894;         // the ad URL answer, owned
extern char g_bn_6ff3f770[0x104];      // version-check archive name
}
// the first error wins (retail takes the module lock around the test and the store)
inline void bnSidFail(uint32_t code) { if (g_bn_6ff3f1f4 == 0) g_bn_6ff3f1f4 = code; }

// strings the handlers hand on
inline constexpr char kBnSidConfiguration[] = "Configuration";
inline constexpr char kBnSidServerList[] = "Server List";
inline constexpr char kBnSidRegVersion[] = "Registration Version";
inline constexpr char kBnSidRegAuthority[] = "Registration Authority";
inline constexpr char kBnSidClientId[] = "Client ID";
inline constexpr char kBnSidClientToken[] = "Client Token";
inline constexpr char kBnSidClientAck[] = "client acknowledged";
inline constexpr char kBnSidClientIdBad[] = "ERROR BDClientId: invalid response";
inline constexpr char kBnSidLogonAck[] = "logon acknowledged";
inline constexpr char kBnSidSendFile[] = "..\\Source\\D2BNClient\\BnSend.cpp";
