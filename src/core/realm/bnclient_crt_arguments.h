#pragma once
#include <cstdint>
struct BnCrtArguments {
    const char* command_line = "";
    char** argv = nullptr;
    int argc = 0;
    char program_name[260] = {};
};
extern "C" BnCrtArguments* bn_crt_arguments();
extern "C" void bn_crt_argument_inputs(const char*);
extern "C" void bn_parse_cmdline(const char*, char**, char*, int*, int*, const uint8_t*);
extern "C" int bn_setargv();
extern "C" const uint8_t* bn_mbctype();
