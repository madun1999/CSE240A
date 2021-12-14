#include <stdio.h>
#include "perceptron_predictor.h"
#include "predictor.h"
#include "common.h"

int16_t perc_w[PERCEPTRONS_COUNT][HISTORY_SIZE]; // perceptron weights



void init_perceptron_predictor() {
    init_GBHR(GBHR_SIZE);
    // init perceptron weight table
    for (uint16_t i = 0 ; i < PERCEPTRONS_COUNT; i++) {
        for (uint16_t j = 0; j < HISTORY_SIZE + 1; j ++) {
            perc_w[i][j] = 0;
        }
    }
}

int32_t calculate_perceptron(uint32_t pc) {
    uint64_t gbhr = get_GBHR();
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
    uint64_t gbhr = get_GBHR();
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

    update_GBHR(outcome);
}

// void clean() {
//     free(perc_w);
// }