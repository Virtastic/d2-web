#pragma once
#include "inflate_fast.h"
#include "stream_algorithms_wave_a.h"
#include "inflate_lifecycle.h"

int32_t D2Client_InflateBlocks(D2ClientInflateBlocks&, D2ClientInflateInput&,
    D2ClientInflateOutput&, const D2ClientInflateAllocator&, int32_t result);
