// 1.13c Fog implementation batch 1: the Safesock wing (Source/Fog/Src/Safesock.cpp) -- the
// WSOCK32 wrappers behind FSafeSock. Every OS call is injected through FogSockApi so a probe can
// replay it; the two module globals are passed in as pointers because a probe owns the blob:
// +0x819C8 (the resolved local-IP string, 16 bytes) and +0x819A8 (the inet_ntoa copy, 16 bytes).
// Prior art: reference/D2MOO/source/Fog/include/Safesock.h (1.10f signatures and the 0x78-byte
// FSafeSock layout) and reference/CE_Database/1.10f/Fog.dll.exports.h (ordinals 10001..10014).
#include <emscripten.h>
#include <cstdint>
#include <cstddef>
#include <cstdio>
#include "hookenv.h"

namespace
{
	struct FSafeSock
	{
		int32_t bConnected;            // +0x00
		int32_t bConnecting;           // +0x04
		uint32_t nSocket;              // +0x08
		uint8_t tCriticalSection[24];  // +0x0C CRITICAL_SECTION
		uint32_t hThread;              // +0x24 HANDLE
		uint32_t nThreadId;            // +0x28
		int32_t bStopping;             // +0x2C
		uint32_t nFogErrorCode;        // +0x30
		uint32_t nWSALastError;        // +0x34
		char szName[64];               // +0x38
	};
	static_assert(offsetof(FSafeSock, nSocket) == 0x08, "FSafeSock nSocket");
	static_assert(offsetof(FSafeSock, tCriticalSection) == 0x0C, "FSafeSock tCriticalSection");
	static_assert(offsetof(FSafeSock, hThread) == 0x24, "FSafeSock hThread");
	static_assert(offsetof(FSafeSock, bStopping) == 0x2C, "FSafeSock bStopping");
	static_assert(offsetof(FSafeSock, nFogErrorCode) == 0x30, "FSafeSock nFogErrorCode");
	static_assert(offsetof(FSafeSock, nWSALastError) == 0x34, "FSafeSock nWSALastError");
	static_assert(offsetof(FSafeSock, szName) == 0x38, "FSafeSock szName");
	static_assert(sizeof(FSafeSock) == 0x78, "FSafeSock size");

	struct FogSockAddrIn
	{
		uint16_t sin_family;
		uint16_t sin_port;
		uint32_t sin_addr;
		uint8_t sin_zero[8];
	};
	static_assert(sizeof(FogSockAddrIn) == 16, "sockaddr_in");

	// FogSockApi: one pointer per external callee, in retail call order.
	struct FogSockApi
	{
		uint32_t (*wsastartup)(uint32_t version, void* data);
		uint32_t (*wsacleanup)();
		uint32_t (*wsagetlasterror)();
		uint32_t (*socket_)(int32_t af, int32_t type, int32_t proto);
		int32_t (*closesocket_)(uint32_t s);
		int32_t (*connect_)(uint32_t s, const void* name, int32_t namelen);
		int32_t (*send_)(uint32_t s, const void* buf, int32_t len, int32_t flags);
		int32_t (*recv_)(uint32_t s, void* buf, int32_t len, int32_t flags);
		int32_t (*select_)(int32_t nfds, void* readfds, void* writefds, void* exceptfds, const void* timeout);
		int32_t (*fdisset)(uint32_t s, const void* set);
		int32_t (*setsockopt_)(uint32_t s, int32_t level, int32_t optname, const void* optval, int32_t optlen);
		int32_t (*ioctlsocket_)(uint32_t s, int32_t cmd, uint32_t* argp);
		int32_t (*getsockname_)(uint32_t s, void* name, int32_t* namelen);
		uint32_t (*gethostname_)(char* name, int32_t namelen);
		void* (*gethostbyname_)(const char* name);
		uint32_t (*inet_addr_)(const char* cp);
		char* (*inet_ntoa_)(uint32_t in);
		uint16_t (*htons_)(uint16_t hostshort);
		FSafeSock* (*allocSlot)(uint32_t size);                       // Fog+0x1B560
		void (*releaseSlot)(FSafeSock** slot, uint32_t line);         // Fog+0x1B0C0
		void (*fatalAlloc)(uint32_t kind, uint32_t line);             // Fog+0x10F30
		void (*validateCS)(void* cs, uint32_t line);                  // Fog+0x11690
		void (*initCS)(void* cs);                                     // InitializeCriticalSection
		void (*enterCS)(void* cs);
		void (*leaveCS)(void* cs);
		void (*deleteCS)(void* cs);
		uint32_t (*createThread)(uint32_t (*proc)(void*), void* param, uint32_t* tid);
		uint32_t (*waitThread)(uint32_t handle, uint32_t ms);
		int32_t (*closeHandle)(uint32_t handle);
		void (*logTimeout)(const char* msg);                          // Fog+0x19100
	};

	// Storm #501 SStrCopy: copy at most max-1 bytes, always terminate.
	void SStrCopy(char* dest, const char* src, uint32_t max)
	{
		uint32_t i = 0;
		if (!max)
			return;
		for (; i + 1 < max && src[i]; ++i)
			dest[i] = src[i];
		dest[i] = 0;
	}
}

// 1.13c Fog+0x16900 (ordinal 10001, SOCKET_WSAStartup): WSAStartup(0x101, &wsaData); TRUE only
// when it returns 0. The 0x190-byte wsaData lives on the caller's stack.
static int32_t fog_safesock_wsastartup(FogSockApi* api)
{
	uint8_t wsaData[0x190];
	return api->wsastartup(0x101, wsaData) == 0 ? 1 : 0;
}

// 1.13c Fog+0x16B10 (ordinal 10004, SOCKET_ConnectEx): allocate and zero an FSafeSock, copy the
// name (SStrCopy, limit 0x40), initialize its critical section and WSAStartup. A socket failure,
// WSA failure or connect failure fills nFogErrorCode (1/2/3) + nWSALastError and frees the slot.
// A non-blocking connect may fail with WSAEWOULDBLOCK 0x2733 and continue; connect() -1 otherwise
// returns NULL. TCP_NODELAY, SO_KEEPALIVE and a 3000 ms SO_SNDTIMEO are set; FIONBIO only when
// bNonBlocking&1. On success bConnected/bConnecting are 1/0 (blocking) and the local address is
// recorded in the module IP string when it is empty or unparsable as an inet_addr. When
// pThreadProc is non-NULL the thread is created with lpParameter and nThreadId.
static FSafeSock* fog_safesock_connect_ex(FogSockApi* api, const char* host, uint16_t port,
                                          uint32_t (*proc)(void*), void* param, const char* name,
                                          int32_t nonBlocking, char* ipString)
{
	FSafeSock* s = api->allocSlot(0x78);
	if (!s)
	{
		api->fatalAlloc(2, 0xC4);
		return nullptr;   // retail's fatal does not return
	}
	for (size_t i = 0; i < sizeof(FSafeSock) / sizeof(uint32_t); ++i)
		reinterpret_cast<uint32_t*>(s)[i] = 0;
	SStrCopy(s->szName, name ? name : "", 0x40);
	api->initCS(s->tCriticalSection);

	uint8_t wsaData[0x190];
	const uint32_t wsaError = api->wsastartup(0x101, wsaData);
	if (wsaError != 0)
	{
		s->nWSALastError = wsaError;
		s->nFogErrorCode = 1;
		api->deleteCS(s->tCriticalSection);
		api->releaseSlot(&s, 0xD6);
		return nullptr;
	}

	s->nSocket = api->socket_(2 /*AF_INET*/, 1 /*SOCK_STREAM*/, 0);
	if (s->nSocket == 0xFFFFFFFFu)
	{
		s->nWSALastError = api->wsagetlasterror();
		s->nFogErrorCode = 2;
		api->deleteCS(s->tCriticalSection);
		api->releaseSlot(&s, 0xE9);
		return nullptr;
	}

	uint32_t one = 1;
	api->setsockopt_(s->nSocket, 6 /*IPPROTO_TCP*/, 1 /*TCP_NODELAY*/, &one, 4);
	api->setsockopt_(s->nSocket, 0xFFFF /*SOL_SOCKET*/, 8 /*SO_KEEPALIVE*/, &one, 4);
	uint32_t timeout = 3000;
	api->setsockopt_(s->nSocket, 0xFFFF /*SOL_SOCKET*/, 0x1005 /*SO_SNDTIMEO*/, &timeout, 4);
	if (nonBlocking & 1)
	{
		uint32_t enable = 1;
		api->ioctlsocket_(s->nSocket, 0x8004667E /*FIONBIO*/, &enable);
	}

	s->bConnecting = 1;
	FogSockAddrIn addr{};
	addr.sin_family = 2;
	addr.sin_port = api->htons_(port);
	addr.sin_addr = api->inet_addr_(host);
	if (api->connect_(s->nSocket, &addr, 16) == -1)
	{
		const uint32_t err = api->wsagetlasterror();
		if (!((nonBlocking & 1) && err == 0x2733 /*WSAEWOULDBLOCK*/))
		{
			s->nWSALastError = err;
			s->nFogErrorCode = 3;
			api->deleteCS(s->tCriticalSection);
			api->releaseSlot(&s, 0x114);
			return nullptr;
		}
	}
	if (!(nonBlocking & 1))
	{
		s->bConnected = 1;
		s->bConnecting = 0;
	}

	if (!ipString[0] || api->inet_addr_(ipString) == 0)
	{
		FogSockAddrIn local{};
		int32_t namelen = 16;
		if (api->getsockname_(s->nSocket, &local, &namelen) != -1)
		{
			const char* ip = api->inet_ntoa_(local.sin_addr);
			char* dst = ipString;
			while ((*dst++ = *ip++) != 0)
				;
		}
	}

	if (proc)
		s->hThread = api->createThread(proc, param, &s->nThreadId);
	return s;
}

// 1.13c Fog+0x16DD0 (ordinal 10003, SOCKET_Connect): SOCKET_ConnectEx(host, port, pThreadProc,
// lpParameter, szName, FALSE) -- the blocking wrapper shares the whole body above.
static FSafeSock* fog_safesock_connect(FogSockApi* api, const char* host, uint16_t port,
                                       uint32_t (*proc)(void*), void* param, const char* name,
                                       char* ipString)
{
	return fog_safesock_connect_ex(api, host, port, proc, param, name, 0, ipString);
}

// 0x16900 (descriptor fog-16900.txt, no arguments): the port's WSAStartup goes through a seam whose call is the recorded hook 0xCDA0
// (stdcall 2 words: 0x101, &WSADATA); the scripted result is blob st (r[16]), the hook log is blob lg (r[14]).
namespace {
HookEnv* gHookEnv;
uint32_t probeWsaStartup(uint32_t version, void* data) {
	return gHookEnv->h(0xCDA0, 0, 0, 0, 0, 0, version, static_cast<uint32_t>(reinterpret_cast<uintptr_t>(data)));
}
// 0x167E0 (descriptor fog-167e0.txt, ECX = in_addr): inet_ntoa through the hook 0xCDA6 (stdcall 1 word); the scripted string is blob nt (r[16]).
char* probeInetNtoa(uint32_t in) {
	return reinterpret_cast<char*>(static_cast<uintptr_t>(gHookEnv->h(0xCDA6, 0, 0, 0, 0, 0, in)));
}
}

extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_fog_16900(uint32_t* r)
{
	const uint32_t sc[4] = { 0xCDA0u, *reinterpret_cast<const uint32_t*>(static_cast<uintptr_t>(r[16])), 0, 0 };
	HookEnv e(reinterpret_cast<uint32_t*>(static_cast<uintptr_t>(r[14])), sc);
	gHookEnv = &e;
	FogSockApi api = {};
	api.wsastartup = probeWsaStartup;
	r[4] = static_cast<uint32_t>(fog_safesock_wsastartup(&api));
	gHookEnv = nullptr;
}

extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_fog_16b10(uint32_t* r)
{
	r[4] = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(fog_safesock_connect_ex(
		reinterpret_cast<FogSockApi*>(static_cast<uintptr_t>(r[0])),
		reinterpret_cast<const char*>(static_cast<uintptr_t>(r[1])), static_cast<uint16_t>(r[2]),
		reinterpret_cast<uint32_t (*)(void*)>(static_cast<uintptr_t>(r[3])),
		reinterpret_cast<void*>(static_cast<uintptr_t>(r[5])),
		reinterpret_cast<const char*>(static_cast<uintptr_t>(r[6])), static_cast<int32_t>(r[7]),
		reinterpret_cast<char*>(static_cast<uintptr_t>(r[8])))));
}

extern "C" __attribute__((weak)) EMSCRIPTEN_KEEPALIVE void d2_fn_fog_16dd0(uint32_t* r)
{
	r[4] = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(fog_safesock_connect(
		reinterpret_cast<FogSockApi*>(static_cast<uintptr_t>(r[0])),
		reinterpret_cast<const char*>(static_cast<uintptr_t>(r[1])), static_cast<uint16_t>(r[2]),
		reinterpret_cast<uint32_t (*)(void*)>(static_cast<uintptr_t>(r[3])),
		reinterpret_cast<void*>(static_cast<uintptr_t>(r[5])),
		reinterpret_cast<const char*>(static_cast<uintptr_t>(r[6])),
		reinterpret_cast<char*>(static_cast<uintptr_t>(r[7])))));
}

// 1.13c Fog+0x17020 (ordinal 10005, SOCKET_Destroy): lock, set bStopping (+0x2C), unlock, wait
// 5000 ms for hThread (+0x24), lock again, CloseHandle and clear hThread, take and clear nSocket
// (+0x08), clear bConnected (+0x00), closesocket (failure: nFogErrorCode 5 + WSAGetLastError),
// delete the critical section and release the slot (Source/Fog/Src/Safesock.cpp:0x173).
static void fog_safesock_destroy(FogSockApi* api, FSafeSock* s)
{
	api->validateCS(s->tCriticalSection, 0x14F);
	api->enterCS(s->tCriticalSection);
	s->bStopping = 1;
	api->leaveCS(s->tCriticalSection);

	if (s->hThread)
		api->waitThread(s->hThread, 5000);

	api->validateCS(s->tCriticalSection, 0x15A);
	api->enterCS(s->tCriticalSection);
	if (s->hThread)
	{
		api->closeHandle(s->hThread);
		s->hThread = 0;
	}
	const uint32_t sock = s->nSocket;
	s->nSocket = 0;
	s->bConnected = 0;
	if (api->closesocket_(sock) == -1)
	{
		s->nWSALastError = api->wsagetlasterror();
		s->nFogErrorCode = 5;
	}
	api->deleteCS(s->tCriticalSection);
	api->releaseSlot(&s, 0x173);
}

// 1.13c Fog+0x16FB0 (ordinal 10006, SOCKET_Receive): refuse a null slot or a stopping one, lock,
// recv(socket, buffer, length, 0), unlock; a zero or negative result records nFogErrorCode 4 and
// the WSA error and is returned as-is.
static int32_t fog_safesock_receive(FogSockApi* api, FSafeSock* s, void* buf, int32_t len)
{
	if (!s || s->bStopping)
		return 0;
	api->validateCS(s->tCriticalSection, 0x183);
	api->enterCS(s->tCriticalSection);
	const int32_t r = api->recv_(s->nSocket, buf, len, 0);
	api->leaveCS(s->tCriticalSection);
	if (r <= 0)
	{
		s->nWSALastError = api->wsagetlasterror();
		s->nFogErrorCode = 4;
	}
	return r;
}

// 1.13c Fog+0x16F00 (ordinal 10007, SOCKET_Send): refuse a null slot or a stopping one, lock,
// send(socket, buffer, length, 0). A full send (result == length) unlocks and returns the count.
// A -1 with WSAEWOULDBLOCK 0x2733 unlocks and returns 0; any other -1 records nFogErrorCode 4 +
// the WSA error and returns -1 WITHOUT unlocking (the 1.13c body leaves the critical section
// held, the safety-lock leak the decompile also shows). Any other short count records the same
// error pair, unlocks and returns 0.
static int32_t fog_safesock_send(FogSockApi* api, FSafeSock* s, const void* buf, int32_t len)
{
	if (!s || s->bStopping)
		return 0;
	api->validateCS(s->tCriticalSection, 0x19F);
	api->enterCS(s->tCriticalSection);
	const int32_t r = api->send_(s->nSocket, buf, len, 0);
	if (r == len)
	{
		api->leaveCS(s->tCriticalSection);
		return r;
	}
	if (r == -1)
	{
		const uint32_t err = api->wsagetlasterror();
		s->nWSALastError = err;
		if (err != 0x2733 /*WSAEWOULDBLOCK*/)
		{
			s->nFogErrorCode = 4;
			return -1;
		}
	}
	else
	{
		s->nWSALastError = api->wsagetlasterror();
		s->nFogErrorCode = 4;
	}
	api->leaveCS(s->tCriticalSection);
	return 0;
}

// 0x17020: slot. 0x16FB0/0x16F00: slot, buffer, length.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_fog_17020(uint32_t* r)
{
	fog_safesock_destroy(reinterpret_cast<FogSockApi*>(static_cast<uintptr_t>(r[0])),
	                     reinterpret_cast<FSafeSock*>(static_cast<uintptr_t>(r[1])));
	r[4] = 0;
}

extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_fog_16fb0(uint32_t* r)
{
	r[4] = static_cast<uint32_t>(fog_safesock_receive(
		reinterpret_cast<FogSockApi*>(static_cast<uintptr_t>(r[0])),
		reinterpret_cast<FSafeSock*>(static_cast<uintptr_t>(r[1])),
		reinterpret_cast<void*>(static_cast<uintptr_t>(r[2])), static_cast<int32_t>(r[3])));
}

extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_fog_16f00(uint32_t* r)
{
	r[4] = static_cast<uint32_t>(fog_safesock_send(
		reinterpret_cast<FogSockApi*>(static_cast<uintptr_t>(r[0])),
		reinterpret_cast<FSafeSock*>(static_cast<uintptr_t>(r[1])),
		reinterpret_cast<const void*>(static_cast<uintptr_t>(r[2])), static_cast<int32_t>(r[3])));
}

namespace
{
	struct FogFdSet
	{
		uint32_t fd_count;
		uint32_t fd_array[64];
	};
	static_assert(sizeof(FogFdSet) == 260, "fd_set");

	struct FogTimeVal
	{
		int32_t tv_sec;
		int32_t tv_usec;
	};
}

// 1.13c Fog+0x16800 (ordinal 10008, SOCKET_Select): a null slot, a stopping slot or an unconnected
// one reads 0. Otherwise test the one socket in a read and an except fd_set with a 100 ms timeout
// (tv_usec 100000). If the slot stopped or disconnected meanwhile, or select() reads 0, the answer
// is 0. select() -1 records nFogErrorCode 4 + the WSA error and answers 1. An except hit answers
// 0, a read hit 1, nothing ready 0.
static int32_t fog_safesock_select(FogSockApi* api, FSafeSock* s)
{
	if (!s || s->bStopping || !s->bConnected)
		return 0;

	FogFdSet readSet{};
	readSet.fd_count = 1;
	readSet.fd_array[0] = s->nSocket;
	FogFdSet exceptSet{};
	exceptSet.fd_count = 1;
	exceptSet.fd_array[0] = s->nSocket;
	FogTimeVal tv{0, 100000};

	const int32_t r = api->select_(0, &readSet, nullptr, &exceptSet, &tv);
	if (s->bStopping || !s->bConnected || r == 0)
		return 0;
	if (r == -1)
	{
		s->nWSALastError = api->wsagetlasterror();
		s->nFogErrorCode = 4;
		return 1;
	}
	if (api->fdisset(s->nSocket, &exceptSet))
		return 0;
	if (api->fdisset(s->nSocket, &readSet))
		return 1;
	return 0;
}

// 1.13c Fog+0x16EB0 (ordinal 10009, SOCKET_SendNoLock): send(socket, buffer, length, 0) with no
// critical section held by the caller. WSAETIMEDOUT 0x274C logs the disconnect notice, reads 0
// and closes the socket; the send result is returned either way.
static int32_t fog_safesock_send_no_lock(FogSockApi* api, FSafeSock* s, const void* buf, int32_t len)
{
	int32_t r = api->send_(s->nSocket, buf, len, 0);
	if (api->wsagetlasterror() == 0x274C /*WSAETIMEDOUT*/)
	{
		api->logTimeout("ERROR: Disconnecting socket due to WSAETIMEDOUT\n");
		r = 0;
		api->closesocket_(s->nSocket);
	}
	return r;
}

// 1.13c Fog+0x16E90 (ordinal 10010, SOCKET_Lock): validate then enter the slot's critical section.
static void fog_safesock_lock(FogSockApi* api, FSafeSock* s)
{
	api->validateCS(s->tCriticalSection, 0x1DF);
	api->enterCS(s->tCriticalSection);
}

// 0x16800/0x16E90: slot. 0x16EB0: slot, buffer, length.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_fog_16800(uint32_t* r)
{
	r[4] = static_cast<uint32_t>(fog_safesock_select(
		reinterpret_cast<FogSockApi*>(static_cast<uintptr_t>(r[0])),
		reinterpret_cast<FSafeSock*>(static_cast<uintptr_t>(r[1]))));
}

extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_fog_16eb0(uint32_t* r)
{
	r[4] = static_cast<uint32_t>(fog_safesock_send_no_lock(
		reinterpret_cast<FogSockApi*>(static_cast<uintptr_t>(r[0])),
		reinterpret_cast<FSafeSock*>(static_cast<uintptr_t>(r[1])),
		reinterpret_cast<const void*>(static_cast<uintptr_t>(r[2])), static_cast<int32_t>(r[3])));
}

extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_fog_16e90(uint32_t* r)
{
	fog_safesock_lock(reinterpret_cast<FogSockApi*>(static_cast<uintptr_t>(r[0])),
	                  reinterpret_cast<FSafeSock*>(static_cast<uintptr_t>(r[1])));
	r[4] = 0;
}

// 1.13c Fog+0x168E0 (ordinal 10011, SOCKET_Unlock): leave the slot's critical section.
static void fog_safesock_unlock(FogSockApi* api, FSafeSock* s)
{
	api->leaveCS(s->tCriticalSection);
}

// 1.13c Fog+0x16A10 (ordinal 10013, GetHostIPAddress): WSAStartup(0x101), resolve the host name,
// take the first address from the hostent's h_addr_list (+0x0C in the 32-bit structure). When that
// is 0, open a UDP socket, connect it to 209.67.136.168:7 and read the local address back with
// getsockname. Format the four bytes little-endian first into the caller's 16-byte string
// ("%d.%d.%d.%d", at most 15 characters), WSACleanup, return that string.
static char* fog_get_host_ip_address(FogSockApi* api, char* ipString)
{
	uint8_t wsaData[0x190];
	api->wsastartup(0x101, wsaData);

	uint32_t addr = 0;
	char hostname[0x100] = {};
	if (api->gethostname_(hostname, 0x100) != 0xFFFFFFFFu)
	{
		const uint8_t* he = reinterpret_cast<const uint8_t*>(api->gethostbyname_(hostname));
		if (he)
		{
			// h_addr_list is the 4-byte pointer at +0x0C; entry 0 is a pointer to the address.
			const uint32_t listPtr = *reinterpret_cast<const uint32_t*>(he + 0x0C);
			const uint32_t entry0 = listPtr ? *reinterpret_cast<const uint32_t*>(static_cast<uintptr_t>(listPtr)) : 0;
			if (entry0)
				addr = *reinterpret_cast<const uint32_t*>(static_cast<uintptr_t>(entry0));
		}
	}

	if (addr == 0)
	{
		const uint32_t s = api->socket_(2 /*AF_INET*/, 2 /*SOCK_DGRAM*/, 0);
		if (s != 0xFFFFFFFFu)
		{
			FogSockAddrIn target{};
			target.sin_family = 2;
			target.sin_port = api->htons_(7);
			target.sin_addr = api->inet_addr_("209.67.136.168");
			if (api->connect_(s, &target, 16) != -1)
			{
				FogSockAddrIn local{};
				int32_t namelen = 16;
				if (api->getsockname_(s, &local, &namelen) != -1)
					addr = local.sin_addr;
			}
			api->closesocket_(s);
		}
	}

	std::snprintf(ipString, 16, "%d.%d.%d.%d", static_cast<int>(addr & 0xFF),
	              static_cast<int>((addr >> 8) & 0xFF), static_cast<int>((addr >> 16) & 0xFF),
	              static_cast<int>((addr >> 24) & 0xFF));
	api->wsacleanup();
	return ipString;
}

// 1.13c Fog+0x167E0 (ordinal 10014, inet_ntoa): WSOCK32 inet_ntoa into a private 16-byte buffer,
// then copy that string (strcpy) into the caller's module buffer and return it.
static char* fog_inet_ntoa(FogSockApi* api, uint32_t in, char* globalBuf)
{
	const char* s = api->inet_ntoa_(in);
	char* dst = globalBuf;
	while ((*dst++ = *s++) != 0)
		;
	return globalBuf;
}

// 0x168E0: slot. 0x16A10: 16-byte output string. 0x167E0: address, 16-byte module buffer.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_fog_168e0(uint32_t* r)
{
	fog_safesock_unlock(reinterpret_cast<FogSockApi*>(static_cast<uintptr_t>(r[0])),
	                    reinterpret_cast<FSafeSock*>(static_cast<uintptr_t>(r[1])));
	r[4] = 0;
}

extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_fog_16a10(uint32_t* r)
{
	r[4] = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(fog_get_host_ip_address(
		reinterpret_cast<FogSockApi*>(static_cast<uintptr_t>(r[0])),
		reinterpret_cast<char*>(static_cast<uintptr_t>(r[1])))));
}

extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_fog_167e0(uint32_t* r)
{
	static char gBuf[16];
	const uint32_t sc[4] = { 0xCDA6u, r[16], 0, 0 };
	HookEnv e(reinterpret_cast<uint32_t*>(static_cast<uintptr_t>(r[14])), sc);
	gHookEnv = &e;
	FogSockApi api = {};
	api.inet_ntoa_ = probeInetNtoa;
	r[4] = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(fog_inet_ntoa(&api, r[0], gBuf)));
	gHookEnv = nullptr;
}
