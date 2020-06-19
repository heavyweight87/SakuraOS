#pragma once

#include <cstdint>

extern uint32_t TOTAL_MEMORY;
extern uint32_t USED_MEMORY;
extern uint8_t MEMORY[1024 * 1024 / 8];

uint32_t physical_alloc(uint32_t count);

void physical_free(uint32_t addr, uint32_t count);

int physical_is_used(uint32_t addr, uint32_t count);

void physical_set_used(uint32_t addr, uint32_t caount);

void physical_set_free(uint32_t addr, uint32_t count);
