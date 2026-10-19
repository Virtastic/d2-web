#pragma once

// Native pointers address a 36-byte retail image; its pointer-shaped slots stay u32.
void* d2_bnclient_gateway_access_assign(void* destination, const void* source);
