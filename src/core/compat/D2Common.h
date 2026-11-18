// Compat: the 1.10f source's D2Common.h is just the image base; ours also pulls Fog.h in, since the 1.10f source
// relies on it arriving transitively through headers we do not vendor.
#pragma once
#include <D2Dll.h>
#include <Fog.h>
