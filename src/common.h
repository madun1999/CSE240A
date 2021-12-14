#ifndef COMMON_H
#define COMMON_H
#include <stdint.h>
#include <stdlib.h>

#define DIGIT(x,i) (((x) >> (i)) & 1)
#define LSB(x,i) ((x) & (i))
#define FULL_BITMASK(n) (((uint64_t) 1 << (n)) - 1)
// global branch history table
void init_GBHR(uint8_t capacity);

uint64_t get_GBHR();

void update_GBHR(uint8_t outcome);


// local branch history table
void init_gshare(uint8_t g_history_bits);

uint8_t get_gshare(uint32_t index);

void update_gshare(uint32_t index, uint8_t outcome);


#endif