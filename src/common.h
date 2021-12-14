#ifndef COMMON_H
#define COMMON_H
#include <stdint.h>
#include <stdlib.h>

#define DIGIT(x,i) (((x) >> (i)) & 1)
#define LSB(x,i) ((x) & i)

// global branch history table
void init_GBHR(uint8_t capacity);

uint64_t get_GBHR();

void update_GBHR(uint8_t outcome);


// local branch history table
void init_BHT(uint32_t capacity);

void get_BHT(uint32_t index);

void update_BHT(uint32_t index, uint8_t outcome);


#endif