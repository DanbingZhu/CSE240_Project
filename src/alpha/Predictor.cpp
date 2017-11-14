/*
 * Predictor.cpp
 *
 *  Created on: Nov 11, 2016
 *      Author: kdhiman
 */

#include "Predictor.h"

Predictor::	Predictor(Traceread* trace, int ADDR_LSB_BITS, int LOC_HIST_SFT_REG_WIDTH, int GLB_HIST_SFT_REG_WIDTH, int STCNTR_WIDTH_LOCAL, int STCNTR_WIDTH_GLOBAL) :
GLB_HIS_REG(GLB_HIST_SFT_REG_INIT, GLB_HIST_SFT_REG_WIDTH)
{
	this->trace = trace;
	this->ADDR_LSB_BITS = ADDR_LSB_BITS;
	this->LOC_HIST_SFT_REG_WIDTH = LOC_HIST_SFT_REG_WIDTH;
	this->GLB_HIST_SFT_REG_WIDTH = GLB_HIST_SFT_REG_WIDTH;
	this->STCNTR_WIDTH_LOCAL = STCNTR_WIDTH_LOCAL;
	this->STCNTR_WIDTH_GLOBAL = STCNTR_WIDTH_GLOBAL;

	CHOOSER_SIZE = 1 << GLB_HIST_SFT_REG_WIDTH; // Chooser size
	GLB_PRED_TAB_SIZE = 1 << GLB_HIST_SFT_REG_WIDTH; // Global predictor table size
	LOC_PRED_TAB_SIZE = 1 << LOC_HIST_SFT_REG_WIDTH; // Local predictor table size
	LOC_HIST_TAB_SIZE = 1 << ADDR_LSB_BITS; // Local history registers table size

}

void Predictor::init_predictor ()
{
	LOC_HIST_TAB.resize(LOC_HIST_TAB_SIZE, BranchHistory(BR_HIST_SFT_REG_INIT, LOC_HIST_SFT_REG_WIDTH)); // Local history table
	GLB_STCNTR.resize(GLB_PRED_TAB_SIZE, SaturatingCounter(STCNTR_INIT, STCNTR_WIDTH_GLOBAL)); // Global prediction saturating counters
	LOC_STCNTR.resize(LOC_PRED_TAB_SIZE, SaturatingCounter(STCNTR_INIT, STCNTR_WIDTH_LOCAL)); // Local prediction saturating counters
	Chooser.resize(CHOOSER_SIZE, SaturatingCounter(STCNTR_INIT, STCNTR_WIDTH_GLOBAL)); // Chooser
}

bool Predictor::make_prediction (unsigned int pc)
{
	int index_pc2lht =  pc % (1<<ADDR_LSB_BITS);
	int index_lht2lp = LOC_HIST_TAB[index_pc2lht].getIndex();
	bool local_prediction = LOC_STCNTR[index_lht2lp].predict();
	int index_ghr = GLB_HIS_REG.getIndex();
	bool global_prediction = GLB_STCNTR[index_ghr].predict();
	bool choice = Chooser[index_ghr].predict();
	if(choice) return global_prediction;
	else return local_prediction;
}

void Predictor::train_predictor (unsigned int pc, bool outcome)
{
	int index_pc2lht = pc % (1<<ADDR_LSB_BITS);
	int index_lht2lp = LOC_HIST_TAB[index_pc2lht].getIndex();
	int index_ghr = GLB_HIS_REG.getIndex();

	bool local_prediction = LOC_STCNTR[index_lht2lp].predict();
	bool global_prediction = GLB_STCNTR[index_ghr].predict();

	LOC_HIST_TAB[index_pc2lht].update(outcome);
	LOC_STCNTR[index_lht2lp].update(outcome);
	GLB_HIS_REG.update(outcome);
	GLB_STCNTR[index_ghr].update(outcome);

	if(local_prediction == outcome && global_prediction != outcome)
		Chooser[index_ghr].update(false);
	if(local_prediction != outcome && global_prediction == outcome)
		Chooser[index_ghr].update(true);
}

int Predictor::getcost(){
	int SIZE = CHOOSER_SIZE * STCNTR_WIDTH_GLOBAL + GLB_PRED_TAB_SIZE * STCNTR_WIDTH_GLOBAL + GLB_HIST_SFT_REG_WIDTH + LOC_HIST_TAB_SIZE * LOC_HIST_SFT_REG_WIDTH + LOC_PRED_TAB_SIZE * STCNTR_WIDTH_LOCAL;
	return SIZE;
}
void Predictor::print_cost(){
	printf("ADDR_BITS=%d, BHR_WIDTH=%d, GHR_WIDTH=%d, STCNTR_WIDTH_LOCAL=%d, Cost of predictor = %d\n",  ADDR_LSB_BITS, LOC_HIST_SFT_REG_WIDTH, GLB_HIST_SFT_REG_WIDTH, STCNTR_WIDTH_LOCAL, getcost());
}


