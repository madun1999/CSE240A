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
        gbhr_cap_mask = FULL_BITMASK(capacity);
    }
}

uint64_t get_GBHR(){
    return gbhr & gbhr_cap_mask;
}

void update_GBHR(uint8_t outcome){
    gbhr = (gbhr << 1 | outcome) & gbhr_cap_mask;
}


// GShare, Depends on Glabal Branch History Table, using 2 bit saturated counter

uint8_t gshare[65536]; // 2 bit saturated counter
uint16_t gshare_pc_mask;
#define TWO_BIT_SAT_PRED(x) (((x) >> 1) & 1)

// gshare
void init_gshare(uint8_t g_history_bits){
    gshare_pc_mask = FULL_BITMASK(g_history_bits);
    for (uint16_t i = 0; i < (1 << g_history_bits); i ++) {
        gshare[i] = 0;
    }
}

// Returns 1 if taken, 0 if not taken.
uint8_t get_gshare(uint32_t pc){
    uint16_t index = (pc & gshare_pc_mask) ^ (get_GBHR() & gshare_pc_mask);
    return TWO_BIT_SAT_PRED(gshare[index]);
}

// Update Gshare only. Do not update GBHT. Call update_GBHR(outcome) separately afterwards.
void update_gshare(uint32_t pc, uint8_t outcome){
    uint16_t index = (pc & gshare_pc_mask) ^ (get_GBHR() & gshare_pc_mask);
    if (outcome == 0) {
        if (gshare[index] == 0) {
            return;
        } else {
            gshare[index] -= 1;
        }
    } else {
        if (gshare[index] == 3) {
            return;
        } else {
            gshare[index] += 1;
        }
    }
}
