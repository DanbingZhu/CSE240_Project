/*
 * Predictor.cpp
 *
 *  Created on: Nov 11, 2016
 *      Author: kdhiman
 */

#include "Predictor.h"

Predictor::Predictor(Traceread* trace, int ADDR_LSB_BITS, int BR_HIST_SFT_REG_WIDTH, int NUM_NEURON, int WEIGHT_WIDTH){

	this->trace = trace;
	this->LSB_BITS = ADDR_LSB_BITS;
	this->BR_HIST_SFT_REG_WIDTH = BR_HIST_SFT_REG_WIDTH;
	this->WEIGHT_WIDTH = WEIGHT_WIDTH;
	PBHT_SIZE = 1 << ADDR_LSB_BITS;
	this->NUM_NEURON = NUM_NEURON;

}

void Predictor::init_predictor ()
{
	GPHT.resize(NUM_NEURON, Neuron(BR_HIST_SFT_REG_WIDTH, WEIGHT_WIDTH));
	PBHT.resize(PBHT_SIZE,   BranchHistory(BR_HIST_SFT_REG_INIT,BR_HIST_SFT_REG_WIDTH));
	/*for (int i = 0; i < NUM_NEURON; ++i) {
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
	int index_neuron = index_pbht2gpht % NUM_NEURON;
	return GPHT[index_neuron ].predict(PBHT[index_pc2pbht].getHist());
}

void Predictor::train_predictor (unsigned int pc, bool outcome)
{
	int index_pc2pbht = pc % (1<<LSB_BITS); // bit operation, get the LSBs of pc
    int index_pbht2gpht = PBHT[index_pc2pbht].getIndex();
    int index_neuron = index_pbht2gpht % NUM_NEURON;
	GPHT[index_neuron].update(PBHT[index_pc2pbht].getHist(), outcome);
    PBHT[index_pc2pbht].update(outcome);
}

int Predictor::getcost(){
	Neuron n(BR_HIST_SFT_REG_WIDTH, WEIGHT_WIDTH);
	int SIZE = PBHT_SIZE * BR_HIST_SFT_REG_WIDTH + NUM_NEURON * n.getSize();
	return SIZE;
}

int Predictor::getNumNeurons(int budget){
	int SIZE = budget - PBHT_SIZE * BR_HIST_SFT_REG_WIDTH;
	return SIZE / GPHT[0].getSize();
}
void Predictor::print_cost(){
	printf("ADDR_BITS=%d, BHR_WIDTH=%d, STCNTR_WIDTH=%d, Cost of predictor = %d\n",  LSB_BITS, BR_HIST_SFT_REG_WIDTH, WEIGHT_WIDTH, getcost());
}


