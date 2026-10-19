#include "bnclient_gateway_access.h"
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

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
void* d2_bnclient_gateway_access_assign(void* destination, const void* source) {
    auto* dst = static_cast<unsigned char*>(destination);
    const auto* src = static_cast<const unsigned char*>(source);
    // Preserve retail's ascending dword loads/stores even for partial overlap.
    // Byte copies avoid C++ struct lifetime/aliasing assumptions about FN blobs.
    for (size_t offset = 0; offset < sizeof(BNGatewayAccess32); offset += 4) {
        uint32_t word;
        std::memcpy(&word, src + offset, sizeof(word));
        std::memcpy(dst + offset, &word, sizeof(word));
    }
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

// ---------------------------------------------------------------------------
// Batch alpha lane: the BNGatewayAccess load/select/parse path (rows s3622-s3635).
// Retail is x86-32, so every import the decompile reaches is a field of the host
// struct below and the wasm image links without any retail thunk.
// ---------------------------------------------------------------------------

struct BnetGatewayHost {
    void (*log_printf)(const char*, ...);                                                     // 0x17420
    void (*log_indent)(int32_t, uint32_t);                                                    // 0x17480
    void (*mem_zero)(void*, uint32_t);                                                        // Storm #494 0x806C
    int (*sfile_load_file)(const char*, void**, int32_t*, int32_t, int32_t);                  // Storm #279 0x803C
    int (*sreg_load_data)(const char*, const char*, uint32_t, uint32_t, uint32_t, uint32_t*); // Storm #421 0x8054
    int (*sreg_save_data)(const char*, const char*, uint32_t, const void*, uint32_t);         // Storm #424 0x8066
    int (*sreg_delete_value)(const char*, const char*, uint32_t);                             // Storm #428 0x805A
    uint32_t (*mem_alloc)(uint32_t, const char*, uint32_t, uint32_t);                         // Storm #401 0x8018
    int (*mem_free)(uint32_t, const char*, uint32_t, uint32_t);                               // Storm #403 0x801E
    uint32_t (*sstr_copy)(char*, const char*, uint32_t);                                      // Storm #501 0x804E
    uint32_t (*sstr_len)(const char*);                                                        // Storm #506 0x8042
    int32_t (*sstr_cmpi)(const char*, const char*, uint32_t);                                 // Storm #509 0x8060
    uint32_t (*get_time_zone_information)(void*);                                             // kernel32
    uint16_t (*get_user_default_lang_id)();                                                   // kernel32
    uint32_t (*caller_return_address)(uint32_t);                                              // Fog #10265 0x80E4
    void (*display_halt)(uint32_t, uint32_t);                                                 // Fog #10024 0x80DE
    void (*terminate)(int32_t);                                                               // 0x1569 _exit
};

int32_t bn_get_system_time_zone(const BnetGatewayHost& host);
const char* bn_gateway_nth(uint32_t self, int32_t index, const BnetGatewayHost& host);
const char* bn_gateway_name(uint32_t self, int32_t index, const BnetGatewayHost& host);
const char* bn_gateway_realm(uint32_t self, int32_t index, const BnetGatewayHost& host);
void bn_set_cur_gateway(uint32_t self, int32_t index);
void bn_gateway_save_and_unload(uint32_t self, const BnetGatewayHost& host);
void bn_gateway_pick_closest_zone(uint32_t self, int32_t gmt, const BnetGatewayHost& host);
void bn_gateway_write_default_list(uint32_t self, const BnetGatewayHost& host);
void bn_gateway_load(uint32_t self, const BnetGatewayHost& host);
void bn_gateway_update_from_ini(uint32_t self, const char* data, const BnetGatewayHost& host);
int32_t bn_strnicmp(const char* str1, const char* str2, uint32_t max_count);
int32_t bn_strtol(const char* str, char** end_ptr, int32_t radix);

namespace {

constexpr const char* kGatewaySourceFile = "..\\Source\\D2BNClient\\BNetGW.cpp";

// 1.13c's TIME_ZONE_INFORMATION; only Bias is read back.
struct TimeZoneInformation {
    int32_t bias;
    uint16_t standard_name[32];
    uint16_t standard_date[8];
    int32_t standard_bias;
    uint16_t daylight_name[32];
    uint16_t daylight_date[8];
    int32_t daylight_bias;
};

// BNGatewayAccess::FindKey (0x14D70) A key match returns the
// first byte after the key's delimiter; scanning stops at the next section header.
const char* bn_find_key(const char* data, const char* key) {
    const char* p = data;
    const char* end = data + std::strlen(data);
    uint32_t length = (uint32_t)std::strlen(key);
    while (*p != '\0' && *p != ']') {
        if (bn_strnicmp(p, key, length) == 0) return p + length + 1;
        p = bn_skip_to_eol(p, end);
        while (p < end && (*p == '\r' || *p == '\n')) ++p;
    }
    return nullptr;
}

// BNGatewayAccess::FindSection (0x14E10) Returns the start of
// the line after the section header.
const char* bn_find_section(const char* data, const char* name) {
    const char* p = data;
    const char* end = data + std::strlen(data);
    uint32_t length = (uint32_t)std::strlen(name);
    for (;;) {
        if (*p == '\0') return nullptr;
        while (*p != '[') {
            if (*p == '\0') std::abort();  // retail: error box then _exit(-1); never vectored
            ++p;
        }
        if (bn_strnicmp(p + 1, name, length) == 0)
            return bn_skip_eol(bn_skip_to_eol(p + length + 2, end), end);
        p += length;
    }
}

// BNGatewayAccess::GMT (0x14F60): the ZONE string after the n-th gateway name, base 10.
int32_t bn_gateway_gmt(uint32_t self, int32_t index, const BnetGatewayHost& host) {
    const char* p = bn_gateway_nth(self, index, host);
    if (*p != '\0') p += host.sstr_len(p) + 1;
    return bn_strtol(p, nullptr, 10);
}

}  // namespace

// 1.13c BNClient+0x14CC0
// BNGatewayAccess::Nth(int): the n-th gateway name in the NUL-separated buffer; a failed walk
// returns the object itself (as char*), never NULL.
const char* bn_gateway_nth(uint32_t self, int32_t index, const BnetGatewayHost& host) {
    BNGatewayAccess32* access = (BNGatewayAccess32*)(uintptr_t)self;
    const char* p = (const char*)(uintptr_t)access->gateway_buffer_ptr_10;
    if (p != nullptr && index > 0 && index <= (int32_t)access->gateway_count_08) {
        int32_t counter = 1;
        int32_t target = index * 3;
        int32_t offset = 0;
        if (1 < target) {
            do {
                if ((int32_t)access->gateway_buffer_bytes_14 <= offset)
                    return (const char*)(uintptr_t)self;
                int32_t length = (int32_t)host.sstr_len(p);
                p += length + 1;
                offset += length + 1;
                counter++;
            } while (counter < target);
        }
        if (offset < (int32_t)access->gateway_buffer_bytes_14 && counter <= target)
            self = (uint32_t)(uintptr_t)p;
    }
    return (const char*)(uintptr_t)self;
}

// 1.13c BNClient+0x14F20
// BNGatewayAccess::Name(int): the gateway's display name, i.e. the string two slots past the
// name Nth returns (name, ZONE, ENU); stops at the first empty string.
const char* bn_gateway_name(uint32_t self, int32_t index, const BnetGatewayHost& host) {
    const char* p = bn_gateway_nth(self, index, host);
    if (*p != '\0') {
        p += host.sstr_len(p) + 1;
        if (*p != '\0') return p + host.sstr_len(p) + 1;
    }
    return p;
}

// 1.13c BNClient+0x14FC0
// BNGatewayAccess::Realm(int): the realm paired with the n-th gateway in the realms buffer,
// matched case-insensitively (Storm #509); "" when the pair is absent.
const char* bn_gateway_realm(uint32_t self, int32_t index, const BnetGatewayHost& host) {
    const char* nth = bn_gateway_nth(self, index, host);
    const char* p = (const char*)(uintptr_t)((BNGatewayAccess32*)(uintptr_t)self)->realms_buffer_ptr_20;
    while (*p != '\0') {
        if (host.sstr_cmpi(p, nth, 0x7FFFFFFFu) == 0) return p + host.sstr_len(p) + 1;
        p += host.sstr_len(p) + 1;
        if (*p == '\0') break;
        p += host.sstr_len(p) + 1;
    }
    return "";
}

// 1.13c BNClient+0x14C90
// BNGatewayAccess::SetCurGateway(int): latch a 1-based gateway index into the 0..99 field; the
// caller's "selection changed" flag is set so SaveAndUnload writes it back.
void bn_set_cur_gateway(uint32_t self, int32_t index) {
    BNGatewayAccess32* access = (BNGatewayAccess32*)(uintptr_t)self;
    if (access->gateway_buffer_ptr_10 != 0 && index > 0 && index <= (int32_t)access->gateway_count_08) {
        if (index > 99) index = 99;
        access->current_gateway_0c = (uint32_t)index;
        access->selection_changed_04 = 1;
    }
}

// 1.13c BNClient+0x14D20
// BNGatewayAccess::GetSystemTimeZone: the host UTC bias in minutes, or 0x1E0 for the Japanese
// and Korean locale pairs (LANGID low word 9, high word 0xC00).
int32_t bn_get_system_time_zone(const BnetGatewayHost& host) {
    TimeZoneInformation tz;
    // The retail instruction stream seeds these before the zero/OS calls; TIME_ZONE_INFORMATION
    // overwrites the whole struct, so they have no observable effect.
    tz.bias = (int32_t)(uintptr_t)(tz.standard_name + 2);
    tz.standard_name[0] = 0x00AC;
    tz.standard_name[1] = 0;
    host.mem_zero(&tz, (uint32_t)sizeof(tz));
    host.get_time_zone_information(&tz);
    uint16_t lang = host.get_user_default_lang_id();
    if ((lang & 0x03FF) == 9 && (lang & 0xFC00) == 0x0C00) return 0x1E0;
    return tz.bias;
}

// 1.13c BNClient+0x15030
// BNGatewayAccess::SaveAndUnload: write the two-digit selection into the buffer's second header
// string, save it to the Configuration registry value, then free both buffers.
void bn_gateway_save_and_unload(uint32_t self, const BnetGatewayHost& host) {
    BNGatewayAccess32* access = (BNGatewayAccess32*)(uintptr_t)self;
    if (access->selection_changed_04 != 0 && access->gateway_buffer_ptr_10 != 0) {
        host.log_printf("Recording gateway list selection");
        char* data = (char*)(uintptr_t)access->gateway_buffer_ptr_10;
        int32_t length = (int32_t)host.sstr_len(data);
        int32_t current = (int32_t)access->current_gateway_0c;
        char* digits = data + 1 + length;
        digits[0] = (char)(current / 10) + '0';
        digits[1] = (char)(current % 10) + '0';
        const char* name = access->override_list_1c == 0 ? "Diablo II Battle.net gateways"
                                                         : "Override Battle.net gateways";
        uint32_t bytes = access->gateway_buffer_bytes_14;
        if (data[bytes - 1] != '\0' || data[bytes - 2] != '\0') {
            host.display_halt(kGatewayMessageData, host.caller_return_address(0xAB));
            host.terminate(-1);
        }
        host.sreg_save_data("Configuration", name, 0x82, data, bytes);
    }
    if (access->gateway_buffer_ptr_10 != 0) {
        host.mem_free(access->gateway_buffer_ptr_10, kGatewaySourceFile, 0xB8, 0);
        access->gateway_buffer_ptr_10 = 0;
        access->gateway_buffer_bytes_14 = 0;
    }
    if (access->realms_buffer_ptr_20 != 0) {
        host.mem_free(access->realms_buffer_ptr_20, kGatewaySourceFile, 0xBE, 0);
        access->realms_buffer_ptr_20 = 0;
    }
}

// 1.13c BNClient+0x15130
// BNGatewayAccess::PickClosestZone(int): pick the gateway whose ZONE is closest to the host GMT
// (in minutes), then latch it. The log varargs are the values the format string names; Ghidra
// drops them because the log routine's prototype is not variadic.
void bn_gateway_pick_closest_zone(uint32_t self, int32_t gmt, const BnetGatewayHost& host) {
    BNGatewayAccess32* access = (BNGatewayAccess32*)(uintptr_t)self;
    host.log_indent(2, 4);
    host.log_printf("Find gateway closest to %02d.%02d", gmt / 60, gmt % 60);
    int32_t best = 1;
    int32_t best_distance = 0x564;
    for (int32_t index = 1; index <= (int32_t)access->gateway_count_08; index++) {
        const char* p = bn_gateway_nth(self, index, host);
        if (*p != '\0') p += host.sstr_len(p) + 1;
        int32_t distance = bn_strtol(p, nullptr, 10) * 60 - gmt;
        if (distance < 0) distance = -distance;
        if (distance < best_distance) {
            best = index;
            best_distance = distance;
        }
    }
    const char* p = bn_gateway_nth(self, best, host);
    if (*p != '\0') p += host.sstr_len(p) + 1;
    int32_t zone = bn_strtol(p, nullptr, 10);
    host.log_printf("Gateway %d (%s at GMT %02d) is closest to %02d.%02d",
                    best, bn_gateway_nth(self, best, host), zone, gmt / 60, gmt % 60);
    host.log_indent(2, 0xFFFFFFFCu);
    bn_set_cur_gateway(self, best);
}

// 1.13c BNClient+0x15560
// BNGatewayAccess::WriteDefaultGatewayList: load Data\Global\gateways.txt and parse it as the
// gateway list; a failed load takes the fatal Fog path with code 0x1B8.
void bn_gateway_write_default_list(uint32_t self, const BnetGatewayHost& host) {
    host.log_printf("Writing default gateway list");
    void* buffer = nullptr;
    int32_t size = 0;
    if (host.sfile_load_file("DATA\\GLOBAL\\gateways.txt", &buffer, &size, 0, 0) == 0) {
        host.display_halt(kGatewayMessageData, host.caller_return_address(0x1B8));
        host.terminate(-1);
    }
    bn_gateway_update_from_ini(self, (const char*)buffer, host);
}

// 1.13c BNClient+0x155D0
// BNGatewayAccess::Load: boot-time gateway-list load -- realms.bin, the two registry lists (an
// override first), the default file when neither exists, then the version/selection checks.
void bn_gateway_load(uint32_t self, const BnetGatewayHost& host) {
    BNGatewayAccess32* access = (BNGatewayAccess32*)(uintptr_t)self;
    if (access->gateway_buffer_ptr_10 != 0) return;
    access->gateway_count_08 = 0;
    access->current_gateway_0c = 0;
    access->realms_buffer_ptr_20 = 0;
    access->gateway_buffer_ptr_10 = 0;
    access->gateway_buffer_bytes_14 = 0;
    access->gateway_list_value_18 = 0;
    access->opaque_00 = 0;
    int32_t size = 0;
    if (host.sfile_load_file("Data\\Global\\Realms.bin", (void**)&access->realms_buffer_ptr_20,
                             &size, 0, 0) == 0) {
        host.display_halt(kGatewayMessageData, host.caller_return_address(0x17A));
        host.terminate(-1);
    }
    access->override_list_1c = 0;
    bn_get_gateway_list(self, "Override Battle.net gateways", host.sreg_load_data, host.mem_alloc,
                        host.mem_free, host.caller_return_address, host.display_halt, host.terminate);
    if (access->gateway_buffer_ptr_10 == 0)
        bn_get_gateway_list(self, "Diablo II Battle.net gateways", host.sreg_load_data, host.mem_alloc,
                            host.mem_free, host.caller_return_address, host.display_halt, host.terminate);
    else
        access->override_list_1c = 1;
    if (access->gateway_buffer_ptr_10 == 0) {
        bn_gateway_write_default_list(self, host);
    } else {
        if (access->gateway_list_value_18 < 1000) {
            host.log_printf("Obsolete gateway list (%d) - autodeleting", access->gateway_list_value_18);
            access->selection_changed_04 = 0;
            bn_gateway_save_and_unload(self, host);
            host.sreg_delete_value("Configuration", "Diablo II Battle.net gateways", 2);
        }
        if (access->gateway_buffer_ptr_10 == 0) bn_gateway_write_default_list(self, host);
    }
    if (access->gateway_list_value_18 > 999) {
        const char* data = (const char*)(uintptr_t)access->gateway_buffer_ptr_10;
        const char* cursor = data;
        int32_t total = 0;
        int32_t records = -2;
        while (total < (int32_t)access->gateway_buffer_bytes_14) {
            int32_t length = (int32_t)host.sstr_len(cursor);
            cursor += length + 1;
            total += length + 1;
            records++;
        }
        if ((records - 1) % 3 != 0) {
            host.display_halt(kGatewayMessageData, host.caller_return_address(0x5E));
            host.terminate(-1);
        }
        int32_t length = (int32_t)host.sstr_len(data);
        char* end = nullptr;
        uint32_t current = (uint32_t)std::strtoul(data + 1 + length, &end, 10);
        access->gateway_count_08 = (uint32_t)((records - 1) / 3);
        access->current_gateway_0c = current;
        access->selection_changed_04 = 0;
        if ((int32_t)current < 1 || (int32_t)access->gateway_count_08 < (int32_t)current) {
            host.log_printf("Auto-picking gateway based on GMT");
            bn_gateway_pick_closest_zone(self, bn_get_system_time_zone(host), host);
        }
        return;
    }
    host.display_halt(kGatewayMessageData, host.caller_return_address(0x4D));
    host.terminate(-1);
}

// 1.13c BNClient+0x15260
// BNGatewayAccess::UpdateGatewaysFromIni(char*): parse a newer ini (Server List Version / VER,
// Server Gateways / <n> / ZONE / ENU) into the NUL-separated buffer, then pick, save and reload.
// Ghidra's stack-variable recovery for this function is poor (several locals show as uninitialized
// pointers); the buffer layout is the one the comments reads.
void bn_gateway_update_from_ini(uint32_t self, const char* data, const BnetGatewayHost& host) {
    BNGatewayAccess32* access = (BNGatewayAccess32*)(uintptr_t)self;
    const char* data_end = data + std::strlen(data);
    const char* version_line = bn_find_section(data, "Server List Version");
    uint32_t fatal_code = 0;
    if (version_line == nullptr) {
        fatal_code = 0x236;
    } else {
        const char* version_text = bn_find_key(version_line, "VER");
        if (version_text == nullptr) {
            fatal_code = 0x23B;
        } else {
            char* version_end = nullptr;
            uint32_t version = (uint32_t)std::strtoul(version_text, &version_end, 10);
            if (version <= access->gateway_list_value_18) return;
            host.log_printf("Parsing new gateway list (version %d)", version);
            int32_t gmt = access->gateway_buffer_ptr_10 == 0
                              ? bn_get_system_time_zone(host)
                              : bn_gateway_gmt(self, (int32_t)access->current_gateway_0c, host);
            access->selection_changed_04 = 0;
            bn_gateway_save_and_unload(self, host);
            access->override_list_1c = 0;
            access->selection_changed_04 = 1;
            access->gateway_list_value_18 = version;

            uint32_t allocation = (uint32_t)((data_end - data) * 2);
            char* buffer = (char*)(uintptr_t)host.mem_alloc(allocation, kGatewaySourceFile, 0x256, 0);
            int32_t digits = std::sprintf(buffer, "%d", version);
            buffer[digits + 1] = '0';
            buffer[digits + 2] = '0';
            buffer[digits + 3] = '\0';
            char* cursor = buffer + digits + 4;
            const char* section = bn_find_section(data, "Server Gateways");
            char number[16];
            int32_t index = 1;
            std::sprintf(number, "%d", index);
            const char* key = bn_find_key(section, number);
            while (key != nullptr) {
                const char* name_end = bn_skip_to_eol(key, data_end);
                host.sstr_copy(cursor, key, (uint32_t)(name_end - key) + 1);
                const char* name_value = key;
                uint32_t name_length = (uint32_t)(name_end - key);
                const char* subsection = bn_find_section(data, cursor);
                cursor += name_length + 1;
                if (subsection != nullptr) {
                    const char* zone = bn_find_key(subsection, "ZONE");
                    if (zone == nullptr) {
                        host.sstr_copy(cursor, "0", 2);
                        cursor += 2;
                    } else {
                        const char* zone_end = bn_skip_to_eol(zone, data_end);
                        host.sstr_copy(cursor, zone, (uint32_t)(zone_end - zone) + 1);
                        cursor += (zone_end - zone) + 1;
                    }
                    const char* localized = bn_find_key(subsection, "ENU");
                    if (localized == nullptr) {
                        host.sstr_copy(cursor, name_value, name_length + 1);
                        cursor += name_length + 1;
                    } else {
                        const char* localized_end = bn_skip_to_eol(localized, data_end);
                        host.sstr_copy(cursor, localized, (uint32_t)(localized_end - localized) + 1);
                        cursor += (localized_end - localized) + 1;
                    }
                }
                index++;
                std::sprintf(number, "%d", index);
                key = bn_find_key(section, number);
            }
            *cursor = '\0';
            uint32_t size = (uint32_t)(cursor - buffer) + 1;
            if (size <= allocation) {
                if (buffer[size - 1] == '\0' && buffer[size - 2] == '\0') {
                    access->gateway_buffer_ptr_10 = (uint32_t)(uintptr_t)buffer;
                    access->gateway_buffer_bytes_14 = size;
                    bn_gateway_pick_closest_zone(self, gmt, host);
                    bn_gateway_save_and_unload(self, host);
                    bn_gateway_load(self, host);
                    return;
                }
                host.display_halt(kGatewayMessageData, host.caller_return_address(0x2A7));
                host.terminate(-1);
            } else {
                host.display_halt(kGatewayMessageData, host.caller_return_address(0x2A5));
                host.terminate(-1);
            }
            // A returning terminate falls through the instruction stream to the 0x23B fatal.
            fatal_code = 0x23B;
        }
    }
    host.display_halt(kGatewayMessageData, host.caller_return_address(fatal_code));
    host.terminate(-1);
}

// 1.13c BNClient+0x0165A
// Visual Studio 2003 __strnicmp: ASCII case-folded compare, at most max_count bytes.
int32_t bn_strnicmp(const char* str1, const char* str2, uint32_t max_count) {
    if (max_count == 0) return 0;
    int32_t c1;
    int32_t c2;
    do {
        uint8_t a = (uint8_t)*str1++;
        uint8_t b = (uint8_t)*str2++;
        c1 = (a >= 'A' && a <= 'Z') ? a + ('a' - 'A') : a;
        c2 = (b >= 'A' && b <= 'Z') ? b + ('a' - 'A') : b;
        if (--max_count == 0 || c1 == 0) break;
    } while (c1 == c2);
    return c1 - c2;
}

// 1.13c BNClient+0x01898
// The CRT's _strtol: skip whitespace, one sign, base prefix, digits; saturate to the 32-bit long
// range instead of wrapping, and set *end_ptr to the input on no conversion.
int32_t bn_strtol(const char* str, char** end_ptr, int32_t radix) {
    const char* p = str;
    while (*p == ' ' || (*p >= '\t' && *p <= '\r')) ++p;
    bool negative = false;
    if (*p == '+' || *p == '-') {
        negative = (*p == '-');
        ++p;
    }
    if (radix == 0) radix = (*p == '0') ? ((p[1] == 'x' || p[1] == 'X') ? 16 : 8) : 10;
    if (radix == 16 && p[0] == '0' && (p[1] == 'x' || p[1] == 'X')) p += 2;
    const uint32_t limit = negative ? 0x80000000u : 0x7FFFFFFFu;
    uint32_t value = 0;
    bool any = false;
    bool overflow = false;
    for (;; ++p) {
        int32_t digit;
        if (*p >= '0' && *p <= '9') digit = *p - '0';
        else if (*p >= 'a' && *p <= 'z') digit = *p - 'a' + 10;
        else if (*p >= 'A' && *p <= 'Z') digit = *p - 'A' + 10;
        else break;
        if (digit >= radix) break;
        any = true;
        if (overflow) continue;
        if (value > (limit - (uint32_t)digit) / (uint32_t)radix) overflow = true;
        else value = value * (uint32_t)radix + (uint32_t)digit;
    }
    if (end_ptr != nullptr) *end_ptr = const_cast<char*>(any ? p : str);
    if (overflow) return negative ? (int32_t)0x80000000u : 0x7FFFFFFF;
    return negative ? (int32_t)(0u - value) : (int32_t)value;
}
