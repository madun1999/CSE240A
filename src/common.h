#ifndef COMMON_H
#define COMMON_H
#include <stdint.h>
#include <stdlib.h>


// global branch history table
void init_GBHR(size_t capacity);

uint16_t get_GBHR();

void update_GBHR(size_t outcome);


// local branch history table
void init_BHT(size_t capacity);

void get_BHT(size_t index);

void update_BHT(size_t index, size_t outcome);


#endif