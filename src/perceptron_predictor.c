#include <stdio.h>
#include <string.h>
#include "perceptron_predictor.h"
#include "predictor.h"
#include "common.h"


int16_t **perc_w; // perceptron weights
int phistoryBits;
int choiceBits;

void init_perceptron_predictor() {
    // init perceptron weight table
    perc_w = calloc(1 << pcIndexBits, sizeof *perc_w);
    for (uint16_t i = 0 ; i < (1 << pcIndexBits); i++) {
        perc_w[i] = calloc(phistoryBits + 1, sizeof **perc_w);
    }
}

int32_t calculate_perceptron(uint32_t pc) {
    uint64_t gbhr = get_GBHR(phistoryBits);
    int32_t y = 0;
    uint32_t perceptron_idx = LSB(pc, FULL_BITMASK(pcIndexBits));

    for (uint16_t i = 0; i < phistoryBits; i ++) {
        y += perc_w[perceptron_idx][i] * DIGIT(gbhr, i);
    }
    y += perc_w[perceptron_idx][phistoryBits]; // bias bit
    return y;
}

uint8_t make_perceptron_prediction(uint32_t pc) {
    int32_t y = calculate_perceptron(pc);
    return (y >= 0) ? TAKEN : NOTTAKEN;
}

// Train the predictor the last executed branch at PC 'pc' and with
// outcome 'outcome' (true indicates that the branch was taken, false
// indicates that the branch was not taken)
//
void train_perceptron_predictor(uint32_t pc, uint8_t outcome) {
    uint64_t gbhr = get_GBHR(phistoryBits);
    int32_t output_value = calculate_perceptron(pc);
    uint8_t predicted = (output_value >= 0) ? TAKEN : NOTTAKEN;
    uint32_t perceptron_idx = LSB(pc, FULL_BITMASK(pcIndexBits));

    if (predicted != outcome || abs(output_value) < THRESHOLD) { // TODO: abs range?
        for (uint16_t i = 0; i < phistoryBits; i ++) {
            perc_w[perceptron_idx][i] += (outcome * 2 - 1) * DIGIT(gbhr, i);
            if (perc_w[perceptron_idx][i] > WEIGHT_MAX ) {
                perc_w[perceptron_idx][i] = WEIGHT_MAX;
            }
            if (perc_w[perceptron_idx][i] < WEIGHT_MIN ) {
                perc_w[perceptron_idx][i] = WEIGHT_MIN;
            }
        }
        perc_w[perceptron_idx][phistoryBits] += (outcome * 2 - 1); // bias bit
        if (perc_w[perceptron_idx][phistoryBits] > WEIGHT_MAX ) {
            perc_w[perceptron_idx][phistoryBits] = WEIGHT_MAX;
        }
        if (perc_w[perceptron_idx][phistoryBits] < WEIGHT_MIN ) {
            perc_w[perceptron_idx][phistoryBits] = WEIGHT_MIN;
        }
    }
}

// void clean() {
//     free(perc_w);
// }

uint8_t *choice_table;
void init_choice_predictor() {
    choice_table = malloc((1 << choiceBits) * sizeof *choice_table);
    memset(choice_table, WN, (1 << choiceBits) * sizeof *choice_table);
}
uint8_t make_choice_prediction(uint32_t pc, uint8_t p1, uint8_t p2) {
    uint32_t idx = get_GBHR(choiceBits);
    uint8_t pred = choice_table[idx];
    if(pred == WN || pred == SN){
        return p1;
    }
    else{
        return p2;
    }
}

void train_choice_predictor(uint32_t pc, uint8_t p1, uint8_t p2, uint8_t outcome) {
    if (p1 != p2) {
        update_table(&choice_table[get_GBHR(choiceBits)], (p2 == outcome) ? TAKEN : NOTTAKEN);
    }
}