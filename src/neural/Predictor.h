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
#include <cmath>
using namespace std;

#define TAKEN true
#define NOT_TAKEN false
#define BR_HIST_SFT_REG_INIT false
#define STCNTR_INIT 0

class Neuron{
	vector<int> weights;
	int N;
	int B;
	bool last_predict;
	int min;
	int max;
	int theta;
	int last_y;
public:
	Neuron(int Num_HBits, int W_Bits){
		N = Num_HBits+1;
		B = W_Bits;
		weights.resize(N, 0);
		last_predict = false;
		max = (1 << (W_Bits-1)) - 1;
		min = -(1 << (W_Bits-1));
		float f_H = N;
		float f_th = f_H*1.93+14;
		theta = f_th;
		last_y=0;
		//printf("max=%d, min=%d, theta=%d\n", max, min, theta);
	}
	bool predict(vector<bool> hist){
		int y = weights[0];
		for (int i = 1; i < weights.size(); ++i) {
			if(hist[i-1]){
				y += weights[i];
			}
			else{
				y -= weights[i];
			}
		}
		last_y = y;
		if(y>0)
			last_predict = true;
		else
			last_predict = false;
		return last_predict;
	}
	void update(vector<bool> hist, bool correct_dir){
		if(last_predict!=correct_dir || (abs(last_y) < theta)){
			int t = weights[0] + 1;
			if(t>max) t = max;
			if(t<min) t = min;
			weights[0] = t;
			for (int i = 1; i < weights.size(); ++i) {
				int w = weights[i];;
				if(hist[i-1]==correct_dir){
					w += 1;
				}else{
					w -= 1;
				}

				if(w>max) w = max;
				if(w<min) w = min;
				weights[i] = w;
			}
		}
	}

	int getSize(){
		return N * B + 1;
	}

	void print(){
		printf("N=%d, B=%d, min=%d, max=%d\n", N, B, min, max);
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
	vector<bool> getHist(){
		uint32_t temp = shift_reg;
		vector<bool> t1;
		for (int i = 0; i < width; ++i) {
			uint32_t x = temp & 1;
			t1.push_back(x==1?true:false);
			temp = temp >> 1;
		}
		return t1;
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
	vector<BranchHistory> PBHT;
	vector<Neuron> GPHT;
	int LSB_BITS;
	int BR_HIST_SFT_REG_WIDTH;
	int WEIGHT_WIDTH;
	int PBHT_SIZE;
	int NUM_NEURON;
public:
	Predictor(Traceread* trace, int ADDR_LSB_BITS, int BR_HIST_SFT_REG_WIDTH, int NUM_NEURON, int WEIGHT_WIDTH);
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
	int getNumNeurons(int budget);

};

#endif /* PREDICTOR_H_ */
