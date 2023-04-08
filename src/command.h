#pragma once

#include "coder.h"

int encode_file(const char *in_file_name, const char *out_file_name);
int decode_file(const char *in_file_name, const char *out_file_name);
int test_file(const char *str, const char *out_file_name);