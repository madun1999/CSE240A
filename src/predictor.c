//========================================================//
//  predictor.c                                           //
//  Source file for the Branch Predictor                  //
//                                                        //
//  Implement the various branch predictors below as      //
//  described in the README                               //
//========================================================//
#include <stdio.h>
#include <string.h>
#include "predictor.h"
#include "perceptron_predictor.h"
#include "common.h"

//
// TODO:Student Information
//
const char *studentName = "NAME: Isabella Liu, Dun Ma";
const char *studentID   = "PID: A59002486, ";
const char *email       = "EMAIL: lal005@ucsd.edu, ";

//------------------------------------//
//      Predictor Configuration       //
//------------------------------------//

// Handy Global for use in output routines
const char *bpName[4] = { "Static", "Gshare",
                          "Tournament", "Custom" };

int ghistoryBits; // Number of bits used for Global History
int lhistoryBits; // Number of bits used for Local History
int pcIndexBits;  // Number of bits used for PC index
int bpType;       // Branch Prediction Type
int verbose;

//------------------------------------//
//      Predictor Data Structures     //
//------------------------------------//

//
//TODO: Add your own Branch Predictor data structures here
//

// GSHARE
uint8_t *gshare_BHT;
uint32_t gshare_HIST;

uint32_t get_gshare_idx(uint32_t pc, uint32_t history){
    uint32_t idx = (pc ^ history) & ((1 << ghistoryBits) - 1);
    return idx;
}

void update_table(uint8_t *cntr, uint8_t outcome) {
    if (outcome == NOTTAKEN) {
        if (*cntr != SN) {
            (*cntr)--;
        }
    } else {
        if (*cntr != ST) {
            (*cntr)++;
        }
    }
}

// TOURNAMENT
// uint32_t tournmt_HIST;
// uint8_t local_pred, global_pred;
uint32_t *tournmt_LOCAL_PHT;
uint8_t *tournmt_LOCAL_BHT;
uint8_t *tournmt_GLOBAL_BHT;
// uint8_t *tournmt_CHOICE_BHT;



//------------------------------------//
//        Predictor Functions         //
//------------------------------------//

// Initialize the predictor
//

void gshare_init() {
    gshare_HIST = 0;
    gshare_BHT = (uint8_t*) malloc(sizeof(uint8_t) * (1 << ghistoryBits));
    memset(gshare_BHT, WN, (1 << ghistoryBits) * sizeof(uint8_t));
}

void tournament_init() {

    // init local tables
    tournmt_LOCAL_PHT = (uint32_t*) malloc(sizeof(uint32_t) * (1 << pcIndexBits));
    memset(tournmt_LOCAL_PHT, 0, (1 << pcIndexBits) * sizeof(uint32_t));

    tournmt_LOCAL_BHT = (uint8_t*) malloc(sizeof(uint8_t) * (1 << lhistoryBits));
    memset(tournmt_LOCAL_BHT, WN, (1 << lhistoryBits) * sizeof(uint8_t));

    // init global tables
    tournmt_GLOBAL_BHT = (uint8_t*) malloc(sizeof(uint8_t) * (1 << ghistoryBits));
    memset(tournmt_GLOBAL_BHT, WN, (1 << ghistoryBits) * sizeof(uint8_t));



}
void
init_predictor()
{
  //
  //TODO: Initialize Branch Predictor Data Structures
  //
    switch (bpType) {
        case STATIC:
            break;
        case GSHARE:
            gshare_init();
            break;

        case TOURNAMENT:
            choiceBits = ghistoryBits;
            init_choice_predictor();
            init_GBHR();
            tournament_init();
            break;

        case CUSTOM:{
            gshare_init();
            init_choice_predictor();
            init_GBHR();
            init_perceptron_predictor();

            //calculate total storage used
            // perceptron storage
            int32_t percp_storage = (1 << pcIndexBits) * phistoryBits * WEIGHT_SIZE;
            //GBHR storage
            int32_t gbhr_storage = phistoryBits;
            //gshare storage
            int32_t gshare_storage = 2 * (1 << ghistoryBits);
            //choice storage
            int32_t choice_storage = 2 * (1 << choiceBits);

            int32_t total_storage = \
                percp_storage + gbhr_storage + gshare_storage + choice_storage;
            printf("Total storage used: %d \n", total_storage);
            printf("Max storage allowed: %d \n", MAX_ALLOWED_STORAGE);
            break;
        }

    }
}

uint8_t make_gshare_prediction(uint32_t pc) {
    uint32_t idx = get_gshare_idx(pc, gshare_HIST);
    uint8_t pred = gshare_BHT[idx];
    return (pred == WN || pred == SN) ? NOTTAKEN : TAKEN;
}

uint8_t make_tournament_prediction(uint32_t pc) {

    //global
    uint32_t global_idx = get_GBHR(ghistoryBits);
    uint8_t global_pred = tournmt_GLOBAL_BHT[global_idx];
    //local
    uint32_t local_PHT_idx = pc & FULL_BITMASK(pcIndexBits);
    uint32_t local_BHT_idx = tournmt_LOCAL_PHT[local_PHT_idx];
    uint8_t local_pred = tournmt_LOCAL_BHT[local_BHT_idx];

    //choice
    return make_choice_prediction(pc, global_pred >= WT, local_pred >= WT);

    // if(choice == WN || choice == SN){
    //     // use global predictor
    //     return (global_pred == WN || global_pred == SN) ? NOTTAKEN : TAKEN;
    // }
    // else{
    //     // use local predictor
    //     return (local_pred == WN || local_pred == SN) ? NOTTAKEN : TAKEN;
    // }
}

// Make a prediction for conditional branch instruction at PC 'pc'
// Returning TAKEN indicates a prediction of taken; returning NOTTAKEN
// indicates a prediction of not taken
//
uint8_t
make_prediction(uint32_t pc)
{
  //
  //TODO: Implement prediction scheme
  //

  // Make a prediction based on the bpType
    switch (bpType) {
        case STATIC:
            return TAKEN;

        case GSHARE:{
            return make_gshare_prediction(pc);
            break;
        }

        case TOURNAMENT:{
            return make_tournament_prediction(pc);
            break;
        }

        case CUSTOM:{
            uint8_t p1 = make_gshare_prediction(pc);
            uint8_t p2 = make_perceptron_prediction(pc);
            return make_choice_prediction(pc, p1, p2);
            // return p2;
            break;
        }
        default:
            break;
    }

  // If there is not a compatable bpType then return NOTTAKEN
   return NOTTAKEN;
}

void train_gshare_predictor(uint32_t pc, uint8_t outcome) {
    uint32_t idx = get_gshare_idx(pc, gshare_HIST);
    update_table(&gshare_BHT[idx], outcome);
    gshare_HIST = ((gshare_HIST << 1) & ((1 << ghistoryBits) - 1)) | outcome;
}

void train_tournament_predictor(uint32_t pc, uint8_t outcome) {
    //global
    uint32_t global_idx = get_GBHR(ghistoryBits);
    uint8_t global_pred = tournmt_GLOBAL_BHT[global_idx];
    //local
    uint32_t local_PHT_idx = pc & FULL_BITMASK(pcIndexBits);
    uint32_t local_BHT_idx = tournmt_LOCAL_PHT[local_PHT_idx];
    uint8_t local_pred = tournmt_LOCAL_BHT[local_BHT_idx];

    //train choice
    train_choice_predictor(pc, global_pred >= WT, local_pred >= WT, outcome);

    //train local
    tournmt_LOCAL_PHT[local_PHT_idx] =
        ((tournmt_LOCAL_PHT[local_PHT_idx] << 1) & FULL_BITMASK(lhistoryBits)) | outcome;
    update_table(&tournmt_LOCAL_BHT[local_BHT_idx], outcome);


    //train global
    update_table(&tournmt_GLOBAL_BHT[global_idx], outcome);
}


// Train the predictor the last executed branch at PC 'pc' and with
// outcome 'outcome' (true indicates that the branch was taken, false
// indicates that the branch was not taken)
//
void
train_predictor(uint32_t pc, uint8_t outcome)
{
  //
  //TODO: Implement Predictor training
  //
    switch (bpType) {
        case STATIC:
            break;

        case GSHARE: {
            train_gshare_predictor(pc, outcome);
            break;
        }

        case TOURNAMENT: {
            train_tournament_predictor(pc, outcome);
            update_GBHR(outcome);
            break;
        }

        case CUSTOM: {
            uint8_t p1 = make_gshare_prediction(pc);
            uint8_t p2 = make_perceptron_prediction(pc);
            train_choice_predictor(pc, p1, p2, outcome);
            train_gshare_predictor(pc, outcome);
            train_perceptron_predictor(pc, outcome);
            update_GBHR(outcome);
            break;
        }
    }
}
