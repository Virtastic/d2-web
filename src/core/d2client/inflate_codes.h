#pragma once
#include "inflate_fast.h"

int32_t D2Client_InflateCodes(D2ClientInflateBlocks&, D2ClientInflateInput&,
                            D2ClientInflateOutput&, int32_t result);

// Test seam (FN probes): replaces the two callees; null members fall back to the real functions.
struct D2ClientInflateCodesCallees {
    int32_t (*fast)(uint32_t, uint32_t, const D2ClientInflateHuft*, const D2ClientInflateHuft*, D2ClientInflateBlocks&, D2ClientInflateInput&);
    int32_t (*flush)(D2ClientInflateWindow&, D2ClientInflateOutput&, int32_t);
};
void D2Client_SetInflateCodesCallees(const D2ClientInflateCodesCallees*);
