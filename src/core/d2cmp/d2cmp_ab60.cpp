// 1.13c D2CMP+0xAB60 (3473 bytes): the HSL-derived shift tables of a palette-shift block.
// x87 at 53-bit precision control: each step below is one double operation (no contraction).
#include <cmath>
#include <cstring>
#include "d2cmp_palstate.h"
namespace {
constexpr double kHueStep = 15.0;           // float 0x6FE30158
constexpr double kDim = (double)0.1f;       // 0x6FE30150
constexpr double kLift = (double)0.2f;      // 0x6FE30120
constexpr double kPastelMul = 0x1.aaaaa98e38e45p-1;  // 0x6FE30148 (0.8333333...)
constexpr double kPastelAdd = 0x1.555554c71c723p-3;  // 0x6FE30140 (0.1666666...)

// W -> RGB (0xA710) -> nearest of the block's 256 entries, one 256-byte table.
void emit(uint8_t* block, const double* w, uint8_t* out) {
    uint8_t rgb[0x400];
    d2cmp_a710(w, rgb);
    for (uint32_t i = 0; i < 0x100; ++i)
        out[i] = (uint8_t)d2cmp_sub_9d30(block, 0x100, rgb[i * 4], rgb[i * 4 + 1], rgb[i * 4 + 2]);
}
void shiftHue(double* e, double shift) {
    double h = shift + e[0];
    e[0] = h;
    if (h > 360.0) e[0] = h - 360.0;
}
}
// 1.13c D2CMP+0x0AB60
// W is a working copy of the caller's 256 {H,S,L} (0xA830 output); entries not rewritten in a pass
// keep their previous value (this matters in the hue-band pass).
//   +0x53500 + k*256 (k 0..23): hue + 15k (wrap above 360).
//   +0x54D00 + k*256: hue + 15k, S = 0.5, L - 0.1f clamped at 0.
//   +0x56500 + k*256: hue + 15k, S = 0.5, L + 0.2f clamped at 1.
//   +0x57D00: S = 0, L * 0.5.     +0x57E00: S = 0, L * 0.8333 + 0.1667 (the stored constants).
//   +0x57F00 + k*256: only entries with 45 < H < 315 are recopied and shifted by 15k; the others keep
//     the +0x57E00 pass's grey values.
//   +0x59800 + k*256 (k 0..11): H = 30k, S = 1, L kept.
//   +0x5A400/+0x5A500/+0x5A600: v = low byte of trunc(sqrt(r*r+g*g+b*b)) of each block entry,
//     nearest of (v,0,0), (0,v,0), (0,0,v).
extern "C" void d2cmp_ab60(uint8_t* block, const double* hsl) {
    double w[0x100 * 3];
    for (uint32_t k = 0; k < 0x18; ++k) {
        double shift = (double)k * kHueStep;
        for (int32_t i = 0; i < 0x100; ++i) { std::memcpy(&w[i * 3], &hsl[i * 3], 24); shiftHue(&w[i * 3], shift); }
        emit(block, w, block + 0x53500 + k * 0x100);
    }
    for (uint32_t k = 0; k < 0x18; ++k) {
        double shift = (double)k * kHueStep;
        for (int32_t i = 0; i < 0x100; ++i) {
            double* e = &w[i * 3];
            std::memcpy(e, &hsl[i * 3], 24);
            shiftHue(e, shift);
            e[1] = 0.5;
            e[2] = e[2] - kDim;
            if (e[2] < 0.0) e[2] = 0.0;
        }
        emit(block, w, block + 0x54d00 + k * 0x100);
    }
    for (uint32_t k = 0; k < 0x18; ++k) {
        double shift = (double)k * kHueStep;
        for (int32_t i = 0; i < 0x100; ++i) {
            double* e = &w[i * 3];
            std::memcpy(e, &hsl[i * 3], 24);
            shiftHue(e, shift);
            e[1] = 0.5;
            e[2] = e[2] + kLift;
            if (e[2] > 1.0) e[2] = 1.0;
        }
        emit(block, w, block + 0x56500 + k * 0x100);
    }
    for (int32_t i = 0; i < 0x100; ++i) { std::memcpy(&w[i * 3], &hsl[i * 3], 24); w[i * 3 + 1] = 0.0; w[i * 3 + 2] = w[i * 3 + 2] * 0.5; }
    emit(block, w, block + 0x57d00);
    for (int32_t i = 0; i < 0x100; ++i) {
        std::memcpy(&w[i * 3], &hsl[i * 3], 24);
        w[i * 3 + 1] = 0.0;
        double t = w[i * 3 + 2] * kPastelMul;
        w[i * 3 + 2] = t + kPastelAdd;
    }
    emit(block, w, block + 0x57e00);
    for (uint32_t k = 0; k < 0x18; ++k) {
        for (int32_t i = 0; i < 0x100; ++i) {
            double h = hsl[i * 3];
            if (!(h > 45.0) || !(h < 315.0)) continue;
            std::memcpy(&w[i * 3], &hsl[i * 3], 24);
            shiftHue(&w[i * 3], (double)k * kHueStep);
        }
        emit(block, w, block + 0x57f00 + k * 0x100);
    }
    for (uint32_t k = 0; k < 0xc; ++k) {
        double hue = (double)k * 30.0;  // float 0x6FE30128
        for (int32_t i = 0; i < 0x100; ++i) { std::memcpy(&w[i * 3], &hsl[i * 3], 24); w[i * 3] = hue; w[i * 3 + 1] = 1.0; }
        emit(block, w, block + 0x59800 + k * 0x100);
    }
    for (uint32_t i = 0; i < 0x100; ++i) {
        const uint8_t* p = block + i * 4;
        int32_t sq = (int32_t)(p[0] * p[0] + p[2] * p[2] + p[1] * p[1]);
        uint8_t v = (uint8_t)(int64_t)std::sqrt((double)sq);
        block[0x5a400 + i] = (uint8_t)d2cmp_sub_9d30(block, 0x100, v, 0, 0);
        block[0x5a500 + i] = (uint8_t)d2cmp_sub_9d30(block, 0x100, 0, v, 0);
        block[0x5a600 + i] = (uint8_t)d2cmp_sub_9d30(block, 0x100, 0, 0, v);
    }
}
