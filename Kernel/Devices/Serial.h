#pragma once

namespace Serial {

void Init(void);
int Write(const char* data, size_t length);
int Write(const char* __restrict format, ...);
void Write(char a);

}

