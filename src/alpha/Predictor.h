/*
 * Predictor.h
 *
 *  Created on: Nov 11, 2016
 *      Author: kdhiman
 */

#ifndef PREDICTOR_H_
#define PREDICTOR_H_
#include "Traceread.h"
#include <vector>
#include <string>
#include <stdio.h>
#include <stdint.h>
using namespace std;

#define TAKEN true
#define NOT_TAKEN false
#define GLB_HIST_SFT_REG_INIT false
#define BR_HIST_SFT_REG_INIT false
#define STCNTR_INIT 0


class GlobalHistory{
	uint32_t shift_reg;
	uint32_t mask;
	int width;
public:
	GlobalHistory(bool initial, int width){
		this->width = width;
		shift_reg = 0;
		mask = 0;
		for (int i = 0; i < width; ++i) {
			shift_reg  = shift_reg << 1;
			shift_reg  += initial;
			mask = mask << 1;
			mask += 1;
		}

	}
	void update(bool branch_result){
		shift_reg = shift_reg << 1;
		shift_reg += branch_result?1:0;
		shift_reg = shift_reg & mask;
	}
	int getIndex(){

		return shift_reg;
	}

	void print(){
		uint32_t temp = shift_reg;
		vector<uint32_t> t1;
		for (int i = 0; i < width; ++i) {
			uint32_t x = temp & 1;
			t1.push_back(x);
			temp = temp >> 1;
		}
		for (int i = width-1; i >=0; --i) {
			printf("%d",t1[i]);
		}
		printf("\t index=%d\n", getIndex());
	}
};

class SaturatingCounter{
	int counter;
	int width;
	int max;
	int min;
public:
	SaturatingCounter(int initial, int width){
		counter = initial;
		max = (1 << width) -1;
		this->width = width;
		this->min = 0;
	}
	void update(bool branch_result){
		if(branch_result == TAKEN){
			if(counter !=max)
				counter++;
		} else {
			if(counter != min)
				counter--;
		}
	}
	bool predict(){
		if(counter>=(max+1)/2){
			return true;
		}
		else{
			return false;
		}
	}

	void print(){
		printf("%d ",counter);
	}
};

class BranchHistory{
	uint32_t shift_reg;
	uint32_t mask;
	int width;
public:
	BranchHistory(bool initial, int width){
		this->width = width;
		shift_reg = 0;
		mask = 0;
		for (int i = 0; i < width; ++i) {
			shift_reg  = shift_reg << 1;
			shift_reg  += initial;
			mask = mask << 1;
			mask += 1;
		}

	}
	void update(bool branch_result){
		shift_reg = shift_reg << 1;
		shift_reg += branch_result?1:0;
		shift_reg = shift_reg & mask;
	}
	int getIndex(){

		return shift_reg;
	}

	void print(){
		uint32_t temp = shift_reg;
		vector<uint32_t> t1;
		for (int i = 0; i < width; ++i) {
			uint32_t x = temp & 1;
			t1.push_back(x);
			temp = temp >> 1;
		}
		for (int i = width-1; i >=0; --i) {
			printf("%d",t1[i]);
		}
		printf("\t index=%d\n", getIndex());
	}
};

class Predictor {
	Traceread* trace;
	GlobalHistory GLB_HIS_REG; // Global/Path history register
	vector<BranchHistory> LOC_HIST_TAB; // Local history table
	vector<SaturatingCounter> GLB_STCNTR; // Global prediction saturating counters
	vector<SaturatingCounter> LOC_STCNTR; // Local prediction saturating counters
	vector<SaturatingCounter> Chooser; // Chooser, decides if we are going to use local predictor or global predictor
	int ADDR_LSB_BITS;
	int GLB_HIST_SFT_REG_WIDTH; // Global history register width
	int LOC_HIST_SFT_REG_WIDTH; // Local history register width
	int STCNTR_WIDTH_LOCAL; // Sctursting counter width, set as 2
	int STCNTR_WIDTH_GLOBAL; // Sctursting counter width, set as 2
	int CHOOSER_SIZE; // Chooser size
	int GLB_PRED_TAB_SIZE; // Global predictor table size
	int LOC_PRED_TAB_SIZE; // Local predictor table size
	int LOC_HIST_TAB_SIZE; // Local history registers table size
public:
	Predictor(Traceread* trace, int ADDR_LSB_BITS, int LOC_HIST_SFT_REG_WIDTH, int GLB_HIST_SFT_REG_WIDTH, int STCNTR_WIDTH_LOCAL, int STCNTR_WIDTH_GLOBAL);
	/*
	  Initialize the predictor.
	 */
	void init_predictor ();

	/*
	  Make a prediction for conditional branch instruction at PC 'pc'.
	  Returning true indicates a prediction of taken; returning false
	  indicates a prediction of not taken.
	 */
	bool make_prediction (unsigned int pc);

	/*
	  Train the predictor the last executed branch at PC 'pc' and with
	  outcome 'outcome' (true indicates that the branch was taken, false
	  indicates that the branch was not taken).
	 */
	void train_predictor (unsigned int pc, bool outcome);

	void print_cost();

	int getcost();

};

#endif /* PREDICTOR_H_ */
