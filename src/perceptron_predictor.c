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
    perc_w = calloc(PERCEPTRONS_COUNT, sizeof *perc_w);
    for (uint16_t i = 0 ; i < PERCEPTRONS_COUNT; i++) {
        perc_w[i] = calloc(HISTORY_SIZE + 1, sizeof **perc_w);
    }
}

int32_t calculate_perceptron(uint32_t pc) {
    uint64_t gbhr = get_GBHR(GBHR_SIZE);
    int32_t y = 0;
    uint32_t perceptron_idx = LSB(pc, PC_LSB_MASK);

    for (uint16_t i = 0; i < HISTORY_SIZE; i ++) {
        y += perc_w[perceptron_idx][i] * DIGIT(gbhr, i);
    }
    y += perc_w[perceptron_idx][HISTORY_SIZE]; // bias bit
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
    uint64_t gbhr = get_GBHR(GBHR_SIZE);
    int32_t output_value = calculate_perceptron(pc);
    uint8_t predicted = (output_value >= 0) ? TAKEN : NOTTAKEN;
    uint32_t perceptron_idx = LSB(pc, PC_LSB_MASK);

    if (predicted != outcome || abs(output_value) < THRESHOLD) { // TODO: abs range?
        for (uint16_t i = 0; i < HISTORY_SIZE; i ++) {
            perc_w[perceptron_idx][i] += (outcome * 2 - 1) * DIGIT(gbhr, i);
            if (perc_w[perceptron_idx][i] > WEIGHT_MAX ) {
                perc_w[perceptron_idx][i] = WEIGHT_MAX;
            }
            if (perc_w[perceptron_idx][i] < WEIGHT_MIN ) {
                perc_w[perceptron_idx][i] = WEIGHT_MIN;
            }
        }
        perc_w[perceptron_idx][HISTORY_SIZE] += (outcome * 2 - 1); // bias bit
        if (perc_w[perceptron_idx][HISTORY_SIZE] > WEIGHT_MAX ) {
            perc_w[perceptron_idx][HISTORY_SIZE] = WEIGHT_MAX;
        }
        if (perc_w[perceptron_idx][HISTORY_SIZE] < WEIGHT_MIN ) {
            perc_w[perceptron_idx][HISTORY_SIZE] = WEIGHT_MIN;
        }
    }
}

// void clean() {
//     free(perc_w);
// }

uint8_t *choice_table;

void init_choice_predictor() {
    choice_table = (uint8_t*) malloc(sizeof(uint8_t) * (1 << choiceBits));
    memset(choice_table, WN, (1 << choiceBits) * sizeof(uint8_t));
}
uint8_t make_choice_prediction(uint32_t pc, uint8_t p1, uint8_t p2) {
    uint32_t idx = get_GBHR(GBHR_SIZE) & ((1 << ghistoryBits) - 1);
    uint8_t pred = choice_table[idx];
    if(pred == WN || pred == SN){
        // use global predictor
        return p1;
    }
    else{
        // use local predictor
        return p2;
    }
}

void train_choice_predictor(uint32_t pc, uint8_t p1, uint8_t p2, uint8_t outcome) {
    if (p1 != p2) {
        update_table(&choice_table[get_GBHR(choiceBits)], (p2 == outcome) ? TAKEN : NOTTAKEN);
    }
}