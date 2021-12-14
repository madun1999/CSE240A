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
#include "predictor.h"

extern int phistoryBits;  // GBHR size
extern int choiceBits;

// #define PERCEPTRONS_COUNT (1 << pcIndexBits)
// #define GBHR_SIZE phistoryBits
// #define BHT_SIZE 1
// #define HISTORY_SIZE GBHR_SIZE
#define THRESHOLD ((int32_t) (1.93 * phistoryBits + 14))
#define WEIGHT_SIZE 8
#define WEIGHT_MAX ((1 << (WEIGHT_SIZE - 1))-1)
#define WEIGHT_MIN (-(WEIGHT_MAX+1))

// #define TOTAL_STORAGE ((1 << pcIndexBits) * phistoryBits * WEIGHT_SIZE + phistoryBits)
#define MAX_ALLOWED_STORAGE 64*1024 + 256
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

void init_choice_predictor();
uint8_t make_choice_prediction(uint32_t pc, uint8_t p1, uint8_t p2);
void train_choice_predictor(uint32_t pc, uint8_t p1, uint8_t p2, uint8_t outcome);

#endif