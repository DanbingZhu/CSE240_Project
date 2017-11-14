/*
 * Predictor.cpp
 *
 *  Created on: Nov 11, 2016
 *      Author: kdhiman
 */

#include "Predictor.h"

Predictor::Predictor(Traceread* trace, int ADDR_LSB_BITS, int BR_HIST_SFT_REG_WIDTH, int STCNTR_WIDTH){

	this->trace = trace;
	this->LSB_BITS = ADDR_LSB_BITS;
	this->BR_HIST_SFT_REG_WIDTH = BR_HIST_SFT_REG_WIDTH;
	this->STCNTR_WIDTH = STCNTR_WIDTH;
	PBHT_SIZE = 1 << ADDR_LSB_BITS;
	GPHT_SIZE = 1 << BR_HIST_SFT_REG_WIDTH;

}

void Predictor::init_predictor ()
{
	GPHT.resize(GPHT_SIZE, SaturatingCounter(STCNTR_INIT, STCNTR_WIDTH));
	PBHT.resize(PBHT_SIZE,   BranchHistory(BR_HIST_SFT_REG_INIT,BR_HIST_SFT_REG_WIDTH));
	/*for (int i = 0; i < GPHT_SIZE; ++i) {
		GPHT[i].print();
	}
	printf("\n");
	for (int i = 0; i < PBHT_SIZE; ++i) {
		PBHT[i].print();
		printf("\n");
	}
	printf("\n");*/
}

bool Predictor::make_prediction (unsigned int pc)
{
	int index_pc2pbht =  pc % (1<<LSB_BITS); // bit operation, get the LSBs of pc
	//printf("pc:%d, index=%d \n", pc%4, index_pc2pbht);
	int index_pbht2gpht = PBHT[index_pc2pbht].getIndex();
	return GPHT[index_pbht2gpht].predict();
}

void Predictor::train_predictor (unsigned int pc, bool outcome)
{
	int index_pc2pbht = pc % (1<<LSB_BITS); // bit operation, get the LSBs of pc
    int index_pbht2gpht = PBHT[index_pc2pbht].getIndex();
	PBHT[index_pc2pbht].update(outcome);
	GPHT[index_pbht2gpht].update(outcome);
}

int Predictor::getcost(){
	int SIZE = PBHT_SIZE * BR_HIST_SFT_REG_WIDTH + GPHT_SIZE * STCNTR_WIDTH;
	return SIZE;
}
void Predictor::print_cost(){
	printf("ADDR_BITS=%d, BHR_WIDTH=%d, STCNTR_WIDTH=%d, Cost of predictor = %d\n",  LSB_BITS, BR_HIST_SFT_REG_WIDTH, STCNTR_WIDTH, getcost());
}


