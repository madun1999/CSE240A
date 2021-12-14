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
uint32_t tournmt_HIST;
uint8_t local_pred, global_pred;
uint8_t *tournmt_LOCAL_BHT;
uint8_t *tournmt_GLOBAL_BHT;
uint8_t *tournmt_CHOICE_BHT;
uint32_t *tournmt_LOCAL_PHT;


//------------------------------------//
//        Predictor Functions         //
//------------------------------------//

// Initialize the predictor
//
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
            gshare_HIST = 0;
            gshare_BHT = (uint8_t*) malloc(sizeof(uint8_t) * (1 << ghistoryBits));
            memset(gshare_BHT, WN, (1 << ghistoryBits) * sizeof(uint8_t));
            break;

        case TOURNAMENT:
            tournmt_HIST = 0;

            tournmt_LOCAL_BHT = (uint8_t*) malloc(sizeof(uint8_t) * (1 << lhistoryBits));
            memset(tournmt_LOCAL_BHT, WN, (1 << lhistoryBits) * sizeof(uint8_t));

            tournmt_GLOBAL_BHT = (uint8_t*) malloc(sizeof(uint8_t) * (1 << ghistoryBits));
            memset(tournmt_GLOBAL_BHT, WN, (1 << ghistoryBits) * sizeof(uint8_t));

            tournmt_CHOICE_BHT = (uint8_t*) malloc(sizeof(uint8_t) * (1 << ghistoryBits));
            memset(tournmt_CHOICE_BHT, WN, (1 << ghistoryBits) * sizeof(uint8_t));

            tournmt_LOCAL_PHT = (uint32_t*) malloc(sizeof(uint32_t) * (1 << pcIndexBits));
            memset(tournmt_LOCAL_PHT, 0, (1 << pcIndexBits) * sizeof(uint32_t));
            break;

        case CUSTOM:{
            init_GBHR(GBHR_SIZE);
            init_perceptron_predictor();
            break;
        }

    }
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
            uint32_t idx = get_gshare_idx(pc, gshare_HIST);
            uint8_t pred = gshare_BHT[idx];
            return (pred == WN || pred == SN) ? NOTTAKEN : TAKEN;
        }

        case TOURNAMENT:{
            uint32_t idx = tournmt_HIST & ((1 << ghistoryBits) - 1);
            uint8_t predictor = tournmt_CHOICE_BHT[idx];
            if(predictor == WN || predictor == SN){
                // use global predictor
                uint32_t global_idx = tournmt_HIST & ((1 << ghistoryBits) - 1);
                global_pred = tournmt_GLOBAL_BHT[global_idx];
                return (global_pred == WN || global_pred == SN) ? NOTTAKEN : TAKEN;
            }
            else{
                // use local predictor
                uint32_t local_PHT_idx = pc & ((1 << pcIndexBits) - 1);
                uint32_t local_BHT_idx = tournmt_LOCAL_PHT[local_PHT_idx];
                local_pred = tournmt_LOCAL_BHT[local_BHT_idx];
                return (local_pred == WN || local_pred == SN) ? NOTTAKEN : TAKEN;
            }
        }

        case CUSTOM:{
            return make_perceptron_prediction(pc);
            break;
        }
        default:
            break;
    }

  // If there is not a compatable bpType then return NOTTAKEN
   return NOTTAKEN;
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
            uint32_t idx = get_gshare_idx(pc, gshare_HIST);
            update_table(&gshare_BHT[idx], outcome);
            gshare_HIST = ((gshare_HIST << 1) & ((1 << ghistoryBits) - 1)) | outcome;
            break;
        }

        case TOURNAMENT: {
            if (local_pred != global_pred)
                update_table(&tournmt_CHOICE_BHT[tournmt_HIST], (local_pred == outcome) ? TAKEN : NOTTAKEN);

            uint32_t local_PHT_idx = pc & ((1 << pcIndexBits) - 1);
            uint32_t local_BHT_idx = tournmt_LOCAL_PHT[local_PHT_idx];
            update_table(&tournmt_LOCAL_BHT[local_BHT_idx], outcome);
            tournmt_LOCAL_PHT[local_PHT_idx] =
                    ((tournmt_LOCAL_PHT[local_PHT_idx] << 1) & ((1 << lhistoryBits) - 1)) | outcome;

            update_table(&tournmt_GLOBAL_BHT[tournmt_HIST], outcome);
            tournmt_HIST = ((tournmt_HIST << 1) & ((1 << ghistoryBits) - 1)) | outcome;
            break;
        }

        case CUSTOM: {
            train_perceptron_predictor(pc, outcome);
            update_GBHR(outcome);
            break;
        }
    }
}
