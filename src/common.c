#include "common.h"

uint64_t gbhr;
uint64_t gbhr_cap_mask;
// global branch history table
// capacity <= 64
void init_GBHR(uint8_t capacity) {
    if (capacity == 64) {
        gbhr_cap_mask = -1;
    } else {
        gbhr = 0;
        gbhr_cap_mask = ((uint64_t) 1 << capacity) - 1;
    }
}

uint64_t get_GBHR(){
    return gbhr & gbhr_cap_mask;
}

void update_GBHR(uint8_t outcome){
    gbhr = (gbhr << 1 | outcome) & gbhr_cap_mask;
}


// local branch history table
void init_BHT(uint32_t capacity){

}

void get_BHT(uint32_t index){

}

void update_BHT(uint32_t index, uint8_t outcome){

}
