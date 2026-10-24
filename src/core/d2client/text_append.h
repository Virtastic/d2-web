#pragma once
#include <cstdint>
struct D2ClientTextEnv {
    const char16_t* (*lang)(uint32_t id);   // D2Lang #10003 (D2Client+0xD1EC thunk): string by id
};
extern "C" {
uint32_t d2_client_2cf20(char16_t* dst, int32_t lo, int32_t hi, uint32_t key, const D2ClientTextEnv* e);
uint32_t d2_client_2d9b0(char16_t* dst, uint32_t key1, uint32_t key2, int32_t value, uint32_t plus, const D2ClientTextEnv* e);
uint32_t d2_client_2c9f0(char16_t* dst, uint32_t key, int32_t value, const D2ClientTextEnv* e);
uint32_t d2_client_2cb60(char16_t* dst, uint32_t a, uint32_t b, const D2ClientTextEnv* e);
uint32_t d2_client_2dbf0(char16_t* dst, uint32_t key, int32_t value, uint32_t plus, const D2ClientTextEnv* e);
uint32_t d2_client_2dca0(char16_t* dst, uint32_t key, int32_t value, uint32_t plus, const D2ClientTextEnv* e);
uint32_t d2_client_2dd50(char16_t* dst, uint32_t key, int32_t value, const D2ClientTextEnv* e);
uint32_t d2_client_2da70(char16_t* dst, int32_t value, uint32_t strA, uint32_t strB, uint32_t plus, const D2ClientTextEnv* e);
}
