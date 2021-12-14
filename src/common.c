#include "common.h"

uint64_t gbhr;
// global branch history table
// capacity <= 64
void init_GBHR() {
        gbhr = 0;
}

uint64_t get_GBHR(uint16_t gbhr_cap){

    return gbhr & FULL_BITMASK(gbhr_cap);
}

void update_GBHR(uint8_t outcome){
    gbhr = gbhr << 1 | outcome;
}
