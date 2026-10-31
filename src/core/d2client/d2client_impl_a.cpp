// Implementation batch A (D2Client leaf and dispatch helpers). Each function is ported from the cached
// 1.13c decompile. Retail ordinals, globals and callees that are not recovered yet come in through
// explicit function-pointer seams, so a vector can pin each one.
#include <array>
#include <cstdint>
#include <cstring>

namespace d2client_impl_a {
using U = uint32_t;
using I = int32_t;

// 1.13c D2Client+0xDA70: n&3 leading bytes one at a time, then n>>2 dwords. 1 when every byte matches.
U mem_equal_DA70(const uint8_t* a, const uint8_t* b, U n) {
    const U lead = n & 3;
    for (U i = 0; i < lead; ++i) {
        if (a[i] != b[i]) return 0;
    }
    a += lead;
    b += lead;
    for (U c = n >> 2; c != 0; --c, a += 4, b += 4) {
        if (std::memcmp(a, b, 4) != 0) return 0;
    }
    return 1;
}

// 1.13c D2Client+0x14810: six-byte message, '^' (0x5E), p3 at +1, p2 as a little-endian dword at +2, then
// the send helper 0x143E0. The send length 6 is the message layout; the 0x143E0 call site in the decompile
// does not show the size register (EBX), so the length is the spec's, not read from the call.
void msg_5e_send_14810(void (*send)(const uint8_t* buf, U len), U p2, uint8_t p3) {
    uint8_t buf[6];
    buf[0] = '^';
    buf[1] = p3;
    std::memcpy(buf + 2, &p2, 4);
    send(buf, 6);
}

// 1.13c D2Client+0x17B0D: roll = 0x1330(seed); roll < 0x4B -> set the 0x1E timer on the sub-object (when the
// unit class is 1 and sub-object +0x14 is set), then tail-call 0x17270; otherwise tail-call 0x173E0(5, 1).
// Guest memory is read and written through the seam; unit is the guest address held in ESI.
struct RangeDispatchHost {
    U (*roll)(U seed);                    // 0x1330
    void (*unitSearch)(U unit);           // 0x17270
    void (*aiStep)(U a, U b);             // 0x173E0
    U (*read32)(U addr);
    void (*write32)(U addr, U value);
};
void range_dispatch_17B0D(const RangeDispatchHost& h, U seed, U unit) {
    const U roll = h.roll(seed);
    if (static_cast<I>(roll) < 0x4B) {
        if (h.read32(unit) == 1) {
            const U sub = h.read32(unit + 0x14);
            if (sub != 0 && sub + 0x30 != 0) h.write32(sub + 0x30, 0x1E);
        }
        h.unitSearch(unit);
        return;
    }
    h.aiStep(5, 1);
}

// 1.13c D2Client+0x59510: the 16 bytes at rec+1 (the last three of them masked to 3 bytes at rec+0x0D, the
// fourth dword's high byte zero) are converted into the name buffer through win2Unicode, and the dword at
// rec+0x11 goes to the tail global.
void record_copy_59510(const uint8_t* rec, void (*win2uni)(uint8_t* dst, const uint8_t* src, U len),
                       uint8_t* nameDst, U* tailDst) {
    uint8_t local[16];
    std::memcpy(local, rec + 1, 12);
    local[12] = rec[0x0D];
    local[13] = rec[0x0E];
    local[14] = rec[0x0F];
    local[15] = 0;
    win2uni(nameDst, local, 16);
    std::memcpy(tailDst, rec + 0x11, 4);
}

// 1.13c D2Client+0x5CC24: scan the table from base while below end, stride 0x20 (eight dwords). Each step first
// calls the gate (ordinal 10044); a non-zero gate with a non-null record whose dword at record+4 equals owner
// is a candidate, and the one with the smallest record+0x34 wins. Returns the entry address minus 8, or 0.
// The owner key is read at record+4 (decompile), not at entry+4 as the spec states.
template <class Gate, class Rd>
U find_entry_5CC24(U base, U end, U owner, Gate gate, Rd rd) {
    U best = 0;
    U e = base;
    do {
        if (gate() != 0) {
            const U rec = rd(e);
            if (rec != 0 && rd(rec + 4) == owner &&
                (best == 0 || rd(rec + 0x34) < rd(rd(best + 8) + 0x34))) {
                best = e - 8;
            }
        }
        e += 0x20;
    } while (e < end);
    return best;
}

// 1.13c D2Client+0x630F0: an 18-dword zeroed record. Dword 0 = p3, dword 13 (+0x34) = the template key
// DAT_6FBCC084. Then ordinal 10041(record, p4, p2, -1, 5, 0). MISMATCH with the spec: the key is at +0x34, not
// +0x14, and the zeroed span is 0x48 bytes (18 dwords), not 0x4C.
void effect_emit_630F0(U p2, U p3, U p4, U templateKey,
                       I (*ord10041)(const U* rec, U p4, U p2, U minusOne, U five, U zero)) {
    std::array<U, 18> rec{};
    rec[0] = p3;
    rec[13] = templateKey;
    ord10041(rec.data(), p4, p2, 0xFFFFFFFFu, 5, 0);
}

// 1.13c D2Client+0xA53E0: EAX == 0 is a retail trap (message 0x961 via ordinal 10265, printed with the string
// at fmt through ordinal 10024, then exit(-1)); otherwise the state table at 0x6FBBA608 is dispatched via 0xA4DA0.
struct StateDispatchHost {
    U (*message)(U id);                   // ordinal 10265
    void (*print)(U fmt, U msg);          // ordinal 10024
    void (*exitProcess)(I code);          // _exit
    void (*tableCall)(U table);           // 0xA4DA0 (table 0x6FBBA608)
};
void state_dispatch_A53E0(const StateDispatchHost& h, U eax) {
    if (eax == 0) {
        const U msg = h.message(0x961);
        h.print(0x6FB8359F, msg);
        h.exitProcess(-1);
        return;
    }
    h.tableCall(0x6FBBA608);
}

// 1.13c D2Client+0xB0090: returns true when the segment from (x, y) to (x+dx, y+dy) is clear of collision mask 4.
// The gate is the squared radius test dx*dx + dy*dy <= r2 (signed compare on the wrapped sum), and the trace is
// ordinal 11076 (ray trace, mask literal 4). dy is EAX in the decompile.
template <class Ray>
bool clear_line_B0090(I dx, I dy, const void* room, I r2, I x, I y, Ray ray) {
    if (room == nullptr) return false;
    const U sum = static_cast<U>(dx) * static_cast<U>(dx) + static_cast<U>(dy) * static_cast<U>(dy);
    if (static_cast<I>(sum) > r2) return false;
    const I from[2] = {x, y};
    const I to[2] = {static_cast<I>(static_cast<U>(x) + static_cast<U>(dx)),
                     static_cast<I>(static_cast<U>(y) + static_cast<U>(dy))};
    return ray(room, from, to, 4) == 0;
}

}  // namespace d2client_impl_a
