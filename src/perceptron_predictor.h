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

#define PERCEPTRONS_COUNT ((1 << 1) - 1)
#define GBHR_SIZE 1
// #define BHT_SIZE 1
// #define HISTORY_SIZE GBHR_SIZE + BHT_SIZE
#define HISTORY_SIZE GBHR_SIZE
#define WEIGHT_SIZE 1
#define THRESHOLD 1
#define PERCEP_MASK 1
//------------------------------------//
//    Predictor Function Prototypes   //
//------------------------------------//

// Initialize the perceptron predictor
//
void init_predictor();

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