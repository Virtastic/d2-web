#include <cstddef>
#include <cstdint>

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
