// src/core/fog/fog_batch_105d0.cpp -- twelve 1.13c Fog.dll rows reimplemented from the cached
// Ghidra decompiles (build-wasm/ghidra-fog/decomp/0x<RVA>.c) in the alpha lane: NAMED, no spec, no
// vector. The exports are identified through the 1.10f Fog definitions/ordinals
// (reference/D2MOO/source/Fog/include/{Fog,Safesock}.h, Fog.1.10f.def):
//   10015 ResolveHostIPAddress  10016 SOCKET_GetError  10017 SOCKET_SetBlockingMode
//   10018 csprintf              10019 InitErrorMgr     10020 (exception-filter install)
//   10022 GetSystemInfo         10023 DisplayAssert    10025 DisplayWarning
//   10026 DisplayError          10027 (deadlock report) 10030 TraceF
// Callees that are separate, unported Fog rows (Fog+0x10F30 error report, Fog+0x184A0 log close,
// Fog+0x113B0 banner, Fog+0x11160 filter) are host seams here and are noted at their call sites.
#include <cstdarg>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace {

// FSafeSock, 1.10f Safesock.h under #pragma pack(1); the 1.13c offsets are the ones the rows use.
struct FogSafeSock {
    int32_t bConnected;            // +0x00
    int32_t bConnecting;           // +0x04
    uint32_t nSocket;              // +0x08
    uint8_t tCriticalSection[24];  // +0x0C CRITICAL_SECTION
    uint32_t hThread;              // +0x24
    uint32_t nThreadId;            // +0x28
    int32_t bStopping;             // +0x2C
    uint32_t nFogErrorCode;        // +0x30
    uint32_t nWSALastError;        // +0x34
    char szName[64];               // +0x38
};
static_assert(offsetof(FogSafeSock, nSocket) == 0x08, "FSafeSock nSocket");
static_assert(offsetof(FogSafeSock, nFogErrorCode) == 0x30, "FSafeSock nFogErrorCode");
static_assert(offsetof(FogSafeSock, nWSALastError) == 0x34, "FSafeSock nWSALastError");
static_assert(sizeof(FogSafeSock) == 0x78, "FSafeSock size");

// The WSOCK32 callees of the resolution/ioctl rows, injected so a probe can replay them. Field
// order is retail call order; the hostent is the 32-bit layout (h_addr_list at +0x0C).
struct FogNetApi {
    void* (*gethostbyname_)(const char* name);           // WSOCK32!#52
    const char* (*inet_ntoa_)(uint32_t addr);            // WSOCK32!#11
    int32_t (*wsastartup_)(uint32_t version, void* data);  // WSOCK32!#115
    int32_t (*wsagetlasterror_)(void);                   // WSOCK32!#111
    void (*wsaunhookblockinghook_)(void);                // WSOCK32!#116
};

typedef int32_t (*fog_ioctlsocket_fn)(uint32_t socket, int32_t cmd, uint32_t* argp);

// Wine/Windows wsock32 ordinal order: 10 inet_addr, 11 inet_ntoa, 12 ioctlsocket, 52 gethostbyname;
// 111 WSAGetLastError, 115 WSAStartup, 116 WSAUnhookBlockingHook.
constexpr int32_t kFionbio = 0x8004667E;
constexpr int32_t kWsaNotInitialised = 0x276D;

// SStrCopy (Storm/Fog import Ordinal_501): bounded copy, always terminated.
void fog_str_copy(char* dest, const char* src, uint32_t max) {
    if (max == 0) return;
    uint32_t i = 0;
    while (i + 1 < max && src[i] != '\0') {
        dest[i] = src[i];
        ++i;
    }
    dest[i] = '\0';
}

// Fog+0x10F30 (ordinal 10028, separate STALLED row): the reporting effects are Win32 file/dialog
// and thread-context calls. The host seam keeps the arguments and format only.
void fog_error_report(int32_t type, const char* file, uint32_t line, const char* format, ...) {
    std::fprintf(stderr, "Fog error %d", static_cast<int>(type));
    if (type != 10) std::fprintf(stderr, " at %s:%u", file ? file : "", line);
    std::fputc(':', stderr);
    va_list ap;
    va_start(ap, format);
    std::vfprintf(stderr, format, ap);
    va_end(ap);
    std::fputc('\n', stderr);
}

// Fog+0x184A0: close the open log FILEs and wait out the error-reporter time window.
void fog_close_logs(void) {}

// Fog+0x11160: the unhandled-exception filter body is a separate row; EXCEPTION_CONTINUE_SEARCH.
int32_t fog_unhandled_filter(void* exceptionPointers) {
    (void)exceptionPointers;
    return 0;
}
int32_t (*g_unhandledFilter)(void*) = nullptr;  // installed by Fog+0x113A0

// Fog+0x113B0 (banner, separate row): writes the machine/version/IP banner to the log.
void fog_write_banner(void) {}

// Fog+0x32EFC: GetSystemInfo's output block; its first dword is retail's initialised flag.
uint32_t g_fog_systemInfo[0x100];
// Fog+0x2FCD8 and Fog+0x32B08: the "Diablo II" program-name buffer and the version buffer.
char g_fog_programName[0x104] = "Diablo II";
char g_fog_version[0x104];
// Fog+0x319C8: the inet_ntoa result buffer returned by ResolveHostIPAddress.
char g_fog_resolvedIp[16];
// Fog+0x5A6E6C, +0x5A6E74, +0x5A6E78: exception callback, Win9x platform flag, initialised flag.
void (*g_fog_exceptionCallback)(void) = nullptr;
uint32_t g_fog_win9x = 0;
uint32_t g_fog_errorMgrInit = 0;

char* fog_resolve_host(const FogNetApi* api, char* host) {
    const uint8_t* he = static_cast<const uint8_t*>(api->gethostbyname_(host));
    uint32_t addr = 0;
    if (he != nullptr) {
        const uint32_t list = *reinterpret_cast<const uint32_t*>(he + 0x0C);
        if (list != 0) addr = *reinterpret_cast<const uint32_t*>(static_cast<uintptr_t>(list));
    }
    if (addr != 0) {
        fog_str_copy(g_fog_resolvedIp, api->inet_ntoa_(addr), sizeof(g_fog_resolvedIp));
        return g_fog_resolvedIp;
    }
    return nullptr;
}

}  // namespace

// 1.13c Fog+0x105D0
// ordinal 10022 GetSystemInfo: the address of the Fog system-info block when its first dword is
// non-zero, otherwise NULL (`-(flag != 0) & Fog+0x32EFC`).
extern "C"
void* fog_sub_105D0(void) {
    return g_fog_systemInfo[0] != 0 ? static_cast<void*>(g_fog_systemInfo) : nullptr;
}

// 1.13c Fog+0x10600
// ordinal 10018 csprintf: _vsprintf into the caller's buffer with the caller's va_list; returns it.
extern "C"
char* fog_sub_10600(char* dest, const char* format, ...) {
    va_list ap;
    va_start(ap, format);
    std::vsprintf(dest, format, ap);
    va_end(ap);
    return dest;
}

// 1.13c Fog+0x11130
// ordinal 10023 DisplayAssert (function list: D2FogAssertOld). WSAUnhookBlockingHook, then the error
// reporter at type 5 with file "" and the caller's line; the caller's second argument is the
// %08x value. The assert message itself is not forwarded. 0x184A0 then closes the logs.
extern "C"
void fog_D2FogAssertOld(const char* msg, const char* file, int32_t line) {
    (void)msg;
    fog_error_report(5, "", static_cast<uint32_t>(line),
                     "Unrecoverable internal error %08x",
                     static_cast<unsigned>(reinterpret_cast<uintptr_t>(file)));
    fog_close_logs();
}

// 1.13c Fog+0x11070
// ordinal 10026 DisplayError(category, msg, file, line): reports the caller's category with file
// "" and line at the fmt's %08x argument; 0x184A0 closes the logs.
extern "C"
void fog_sub_11070(int32_t category, const char* msg, const char* file, int32_t line) {
    (void)msg;
    fog_error_report(category, "", static_cast<uint32_t>(line),
                     "Unrecoverable internal error %08x",
                     static_cast<unsigned>(reinterpret_cast<uintptr_t>(file)));
    fog_close_logs();
}

// 1.13c Fog+0x110A0
// ordinal 10027: deadlock report(thread, ticks). The thread handle takes the reporter's "file"
// slot (type 10 reinterprets it) and the line is the fixed 0x780; the format carries both values.
extern "C"
void fog_sub_110A0(uint32_t thread, int32_t ticks) {
    fog_error_report(10, reinterpret_cast<const char*>(static_cast<uintptr_t>(thread)), 0x780,
                     "Deadlocked in thread %X for %d ticks", thread, static_cast<int>(ticks));
    fog_close_logs();
}

// 1.13c Fog+0x110D0
// ordinal 10025 DisplayWarning: same shape as DisplayAssert at type 7.
extern "C"
void fog_sub_110D0(const char* msg, const char* file, int32_t line) {
    (void)msg;
    fog_error_report(7, "", static_cast<uint32_t>(line),
                     "Unrecoverable internal error %08x",
                     static_cast<unsigned>(reinterpret_cast<uintptr_t>(file)));
    fog_close_logs();
}

// 1.13c Fog+0x113A0
// ordinal 10020: SetUnhandledExceptionFilter(Fog+0x11160). The filter body is a separate row, so
// the port installs a stand-in that returns EXCEPTION_CONTINUE_SEARCH.
extern "C"
void fog_sub_113A0(void) {
    g_unhandledFilter = fog_unhandled_filter;
}

// 1.13c Fog+0x11860
// ordinal 10019 InitErrorMgr(programName, exceptionCallback, version, logToFile). Both strings are
// copied into the module buffers (0x104 bytes each) before anything else. On the first call retail
// also probes the platform, creates two critical sections, hooks the memory log, installs the
// exception filter, self-checks the bit masks, registers the atexit walker and stores the callback;
// those callees are separate rows and are not replayable here. The banner (0x113B0) is likewise a
// separate row.
extern "C"
void fog_sub_11860(const char* programName, void (*exceptionCallback)(void), const char* version,
                   int32_t logToFile) {
    fog_str_copy(g_fog_programName, programName, sizeof(g_fog_programName));
    if (version != nullptr) fog_str_copy(g_fog_version, version, sizeof(g_fog_version));
    if (g_fog_errorMgrInit == 0) {
        g_fog_errorMgrInit = 1;
        g_fog_win9x = 0;  // GetVersionExA: the port never reports a Win9x platform
        g_fog_exceptionCallback = exceptionCallback;
        if (logToFile != 0) fog_write_banner();
    } else if (logToFile != 0) {
        fog_write_banner();
    }
}

// 1.13c Fog+0x167A0
// ordinal 10017 SOCKET_SetBlockingMode: ioctlsocket(socket, FIONBIO, &nonBlocking). The result is
// discarded and retail returns TRUE unconditionally.
extern "C"
int32_t fog_sub_167A0(FogSafeSock* sock, uint32_t nonBlocking, fog_ioctlsocket_fn ioctlsocket_) {
    uint32_t mode = nonBlocking;
    ioctlsocket_(sock->nSocket, kFionbio, &mode);
    return 1;
}

// 1.13c Fog+0x167C0
// ordinal 10016 SOCKET_GetError: copy the socket's Fog error code (+0x30) and WSA last error
// (+0x34) into the caller's dwords and return TRUE.
extern "C"
int32_t fog_sub_167C0(FogSafeSock* sock, uint32_t* fogError, uint32_t* wsaLastError) {
    *fogError = sock->nFogErrorCode;
    *wsaLastError = sock->nWSALastError;
    return 1;
}

// 1.13c Fog+0x16930
// ordinal 10015 ResolveHostIPAddress(host): "a.b.c.d" is returned as-is when sscanf reads four
// integers; otherwise gethostbyname first, then (only on WSANOTINITIALISED) WSAStartup(0x101) and
// one retry. On success the first h_addr_list entry goes through inet_ntoa into Fog+0x319C8 and
// that buffer is returned; on failure NULL.
extern "C"
char* fog_sub_16930(const FogNetApi* api, char* host) {
    int32_t a = 0;
    int32_t b = 0;
    int32_t c = 0;
    int32_t d = 0;
    if (std::sscanf(host, "%d.%d.%d.%d", &a, &b, &c, &d) == 4) return host;

    char* resolved = fog_resolve_host(api, host);
    if (resolved != nullptr) return resolved;

    if (api->wsagetlasterror_() == kWsaNotInitialised) {
        uint8_t wsaData[0x190] = {};
        if (api->wsastartup_(0x101, wsaData) == 0) {
            resolved = fog_resolve_host(api, host);
            if (resolved != nullptr) {
                api->wsaunhookblockinghook_();
                return resolved;
            }
            api->wsaunhookblockinghook_();
        }
    }
    return nullptr;
}

// 1.13c Fog+0x190A0
// ordinal 10030 TraceF(fileSubName, format, ...): a NULL format is fatal (reporter type 6 at line
// 0x129, then 0x184A0 and _exit(-1)); otherwise the message goes to the named log, defaulting the
// name when none is given. Log-file I/O is host state, so the port writes the formatted text to
// stderr.
extern "C"
void fog_sub_190A0(const char* fileSubName, const char* format, ...) {
    if (format == nullptr) {
        fog_error_report(6, "", 0x129, "Unrecoverable internal error %08x", 0u);
        fog_close_logs();
        std::_Exit(-1);
    }
    (void)fileSubName;
    va_list ap;
    va_start(ap, format);
    std::vfprintf(stderr, format, ap);
    va_end(ap);
}
