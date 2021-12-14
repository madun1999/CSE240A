//========================================================//
//  perceptron_predictor.h                                //
//  Header file for the Perceptron Branch Predictor       //
//                                                        //
//  Includes function prototypes                          //
//========================================================//

#ifndef PERCEPTRON_PREDICTOR_H
#define PERCEPTRON_PREDICTOR_H

#include <stdint.h>
#include <stdlib.h>

#define PC_LSB_COUNT 6
#define PERCEPTRONS_COUNT (1 << PC_LSB_COUNT)
#define PC_LSB_MASK (PERCEPTRONS_COUNT - 1)
#define GBHR_SIZE 63
// #define BHT_SIZE 1
// #define HISTORY_SIZE GBHR_SIZE + BHT_SIZE
#define HISTORY_SIZE GBHR_SIZE
#define THRESHOLD 135
#define WEIGHT_SIZE 9
#define WEIGHT_MAX ((1 << (WEIGHT_SIZE - 1))-1)
#define WEIGHT_MIN (-(WEIGHT_MAX+1))

#define TOTAL_STORAGE (PERCEPTRONS_COUNT * HISTORY_SIZE * WEIGHT_SIZE + GBHR_SIZE)
#define MAX_ALLOWED_STORAGE (64*1024 + 256)
//------------------------------------//
//    Predictor Function Prototypes   //
//------------------------------------//

// Initialize the perceptron predictor
//
void init_perceptron_predictor();

// Make a prediction for conditional branch instruction at PC 'pc'
// Returning TAKEN indicates a prediction of taken; returning NOTTAKEN
// indicates a prediction of not taken
//
uint8_t make_perceptron_prediction(uint32_t pc);

// Train the predictor the last executed branch at PC 'pc' and with
// outcome 'outcome' (true indicates that the branch was taken, false
// indicates that the branch was not taken)
//
void train_perceptron_predictor(uint32_t pc, uint8_t outcome);

#endif