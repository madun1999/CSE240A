#ifndef COMMON_H
#define COMMON_H
#include <stdint.h>
#include <stdlib.h>

#define DIGIT(x,i) (((x) >> (i)) & 1)
#define LSB(x,i) ((x) & i)
#define FULL_BITMASK(n) (((uint64_t) 1 << (n)) - 1)

// global branch history table
void init_GBHR();

uint64_t get_GBHR(uint16_t gbhr_cap_mask);

void update_GBHR(uint8_t outcome);


#endif