/*
 * Predictor.cpp
 *
 *  Created on: Nov 11, 2016
 *      Author: kdhiman
 */

#include "Predictor.h"

Predictor::Predictor(Traceread* trace, int ADDR_LSB_BITS, int GL_HIST_SFT_REG_WIDTH, int STCNTR_WIDTH) : 
GHR(GL_HIST_SFT_REG_INIT, GL_HIST_SFT_REG_WIDTH)
{
	this->trace = trace;
	this->LSB_BITS = ADDR_LSB_BITS;
	this->GL_HIST_SFT_REG_WIDTH = GL_HIST_SFT_REG_WIDTH;
	this->STCNTR_WIDTH = STCNTR_WIDTH;
	STCNTR_TAB_SIZE = 1 << ADDR_LSB_BITS;
}

void Predictor::init_predictor ()
{
	SCT.resize(STCNTR_TAB_SIZE, SaturatingCounter(STCNTR_INIT, STCNTR_WIDTH));
}

bool Predictor::make_prediction (unsigned int pc)
{
	uint32_t addr = (pc % (1<<LSB_BITS));
	uint32_t hist = GHR.getIndex();
	hist = hist << (LSB_BITS - GL_HIST_SFT_REG_WIDTH);
	uint32_t index =  addr^hist;


	return SCT[index].predict();
}

void Predictor::train_predictor (unsigned int pc, bool outcome)
{
	//int index =  (pc % (1<<LSB_BITS)) ^ GHR.getIndex();
	uint32_t addr = (pc % (1<<LSB_BITS));
	uint32_t hist = GHR.getIndex();
	hist = hist << (LSB_BITS - GL_HIST_SFT_REG_WIDTH);
	uint32_t index =  addr^hist;

	SCT[index].update(outcome);
	GHR.update(outcome);
}

int Predictor::getcost(){
	int SIZE = GL_HIST_SFT_REG_WIDTH + STCNTR_TAB_SIZE * STCNTR_WIDTH;
	return SIZE;
}
void Predictor::print_cost(){
	printf("ADDR_BITS=%d, GHR_WIDTH=%d, STCNTR_WIDTH=%d, Cost of predictor = %d\n",  LSB_BITS, GL_HIST_SFT_REG_WIDTH, STCNTR_WIDTH, getcost());
}


