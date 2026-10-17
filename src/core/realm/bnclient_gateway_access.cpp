#include <cstddef>
#include <cstdint>
#include <cstdlib>

namespace {

// Retail BNClient is x86-32. Keep its pointer-shaped slots as uint32_t values so this
// compatibility image stays 36 bytes in wasm64 as well as on the host.
struct BNGatewayAccess32 {
    uint32_t opaque_00;
    uint32_t selection_changed_04;
    uint32_t gateway_count_08;
    uint32_t current_gateway_0c;
    uint32_t gateway_buffer_ptr_10;
    uint32_t gateway_buffer_bytes_14;
    uint32_t gateway_list_value_18;
    uint32_t override_list_1c;
    uint32_t realms_buffer_ptr_20;
};

static_assert(sizeof(uint32_t) == 4);
static_assert(offsetof(BNGatewayAccess32, opaque_00) == 0x00);
static_assert(offsetof(BNGatewayAccess32, selection_changed_04) == 0x04);
static_assert(offsetof(BNGatewayAccess32, gateway_count_08) == 0x08);
static_assert(offsetof(BNGatewayAccess32, current_gateway_0c) == 0x0c);
static_assert(offsetof(BNGatewayAccess32, gateway_buffer_ptr_10) == 0x10);
static_assert(offsetof(BNGatewayAccess32, gateway_buffer_bytes_14) == 0x14);
static_assert(offsetof(BNGatewayAccess32, gateway_list_value_18) == 0x18);
static_assert(offsetof(BNGatewayAccess32, override_list_1c) == 0x1c);
static_assert(offsetof(BNGatewayAccess32, realms_buffer_ptr_20) == 0x20);
static_assert(sizeof(BNGatewayAccess32) == 0x24);

}  // namespace

// 1.13c BNClient+0x01050
BNGatewayAccess32* d2_bnclient_gateway_access_assign(
    BNGatewayAccess32* destination, const BNGatewayAccess32* source) {
    destination->opaque_00 = source->opaque_00;
    destination->selection_changed_04 = source->selection_changed_04;
    destination->gateway_count_08 = source->gateway_count_08;
    destination->current_gateway_0c = source->current_gateway_0c;
    destination->gateway_buffer_ptr_10 = source->gateway_buffer_ptr_10;
    destination->gateway_buffer_bytes_14 = source->gateway_buffer_bytes_14;
    destination->gateway_list_value_18 = source->gateway_list_value_18;
    destination->override_list_1c = source->override_list_1c;
    destination->realms_buffer_ptr_20 = source->realms_buffer_ptr_20;
    return destination;
}

// 1.13c BNClient+0x14EE0 BNGatewayAccess::GetBattlenetGatewayList
// The callee (GetGatewayList, 0x14BB0) is a parameter: it reads the host config, is UNMAPPED, and `_exit(-1)`s on a
// preset this+0x10, so the caller is a pure function of (this, callee) and the vector hooks it.
void bn_get_battlenet_gateway_list(uint32_t self,
                                   void (*get_gateway_list)(uint32_t, const char*)) {
    BNGatewayAccess32* access = (BNGatewayAccess32*)(uintptr_t)self;
    access->override_list_1c = 0;
    get_gateway_list(self, "Override Battle.net gateways");
    if (access->gateway_buffer_ptr_10 != 0) {
        access->override_list_1c = 1;
        return;
    }
    get_gateway_list(self, "Diablo II Battle.net gateways");
}

// 1.13c BNClient+0x14BB0 BNGatewayAccess::GetGatewayList
// Loads one registry-backed value (Storm #421 SRegLoadData) into an SMemAlloc'd buffer and parses its first
// string as a decimal version. All six import/CRT callees are parameters; _strtoul is plain base-10 strtoul.
// A returning `terminate` falls through into the normal body, as retail's instruction stream does when `_exit`
// is stubbed (retail itself never returns from it).
constexpr uint32_t kGatewayMessageData = 0x1A813u;
void bn_get_gateway_list(
    uint32_t self, const char* name,
    int (*load_data)(const char*, const char*, uint32_t, uint32_t, uint32_t, uint32_t*),
    uint32_t (*mem_alloc)(uint32_t, const char*, uint32_t, uint32_t),
    int (*mem_free)(uint32_t, const char*, uint32_t, uint32_t),
    uint32_t (*caller_return_address)(uint32_t),
    void (*display_halt)(uint32_t, uint32_t),
    void (*terminate)(int)) {
    static constexpr const char* kSection = "Configuration";
    static constexpr const char* kSourceFile = "..\\Source\\D2BNClient\\BNetGW.cpp";
    BNGatewayAccess32* access = (BNGatewayAccess32*)(uintptr_t)self;

    if (access->gateway_buffer_ptr_10 != 0) {
        display_halt(kGatewayMessageData, caller_return_address(0x138));
        terminate(-1);
    }
    uint32_t size = 0;
    if (!load_data(kSection, name, 0x82, 0, 0, &size)) return;
    if (size == 0) return;
    access->gateway_buffer_bytes_14 = size;
    uint32_t buffer = mem_alloc(size, kSourceFile, 0x148, 0);
    access->gateway_buffer_ptr_10 = buffer;
    if (!load_data(kSection, name, 0x82, buffer, size, nullptr)) {
        mem_free(buffer, kSourceFile, 0x150, 0);
        access->gateway_buffer_ptr_10 = 0;
        access->gateway_buffer_bytes_14 = 0;
    }
    access->gateway_list_value_18 = 0;
    if (access->gateway_buffer_ptr_10 != 0) {
        char* start = (char*)(uintptr_t)access->gateway_buffer_ptr_10;
        char* end = start;
        unsigned long value = std::strtoul(start, &end, 10);
        access->gateway_list_value_18 = (uint32_t)value;
        if (end == start) access->gateway_list_value_18 = 0;
    }
}

// 1.13c BNClient+0x14B60 BNGatewayAccess::GetBattlenetRealmsList
// Boot-time load of Data\Global\Realms.bin into this+0x20 through Storm #279 SFileLoadFile (thunk 0x803C). All four
// callees are parameters: three are import thunks into Storm/Fog and one is the CRT's _exit, so the body is a pure
// function of (this, callees) and the vector hooks them. A nonzero load means success; a zero result takes 1.13c's
// fatal tail -- Fog #10265 (FOG_CallerReturnAddress) with code 0x17A, that word handed to Fog #10024
// (FOG_DisplayHalt) together with the module data word BNClient+0x1A813, then _exit(-1). kRealmsMessageData is the
// retail RVA of that data word: BNClient is a wasm-incompatible 32-bit image here, so it is carried as an RVA and
// the probe tags it 0xB2000000|rva.
constexpr uint32_t kRealmsMessageData = 0x1A813u;
void bn_get_battlenet_realms_list(
    uint32_t self,
    int (*load_realms)(const char* name, uint32_t* p_buffer, uint32_t* p_size, uint32_t reserved4, uint32_t reserved5),
    uint32_t (*caller_return_address)(uint32_t code),
    void (*display_halt)(uint32_t data, uint32_t word),
    void (*terminate)(int code)) {
    BNGatewayAccess32* access = (BNGatewayAccess32*)(uintptr_t)self;
    uint32_t size = 0;
    if (load_realms("Data\\Global\\Realms.bin", &access->realms_buffer_ptr_20, &size, 0, 0) != 0) return;
    display_halt(kRealmsMessageData, caller_return_address(0x17A));
    terminate(-1);
}

// 1.13c BNClient+0x14AA0 SkipEOL / +0x14AD0 SkipToEOL
const char* bn_skip_eol(const char* p, const char* end) {
  if (p < end)
    for (; *p; ) { if (*p != '\r' && *p != '\n') return p; if (end <= ++p) return p; }
  return p;
}
const char* bn_skip_to_eol(const char* p, const char* end) {
  while (p < end && *p && *p != '\r' && *p != '\n') p++;
  return p;
}
