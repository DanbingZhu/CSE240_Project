//============================================================================
// Name        : CSE-240A-project.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================
#include <omp.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <netinet/in.h>
#include "Traceread.h"
#include "Predictor.h"
#include <fstream>
#include <sstream>

using namespace std;

class Range{
public:
	int max;
	int min;
	Range(int min, int max){
		this->max = max;
		this-> min = min;
	}
};

class Params{
public:
	Range* addr_bits;
	Range* hist_bits;
	Range* num_neurons;
	Range* sat_cntr;
	Params(Range* addr_bits,Range* hist_bits, Range* num_neurons, Range* sat_cntr){
		this->addr_bits = addr_bits;
		this->hist_bits = hist_bits;
		this->num_neurons = num_neurons;
		this->sat_cntr = sat_cntr;
	}
};


int main_predictor(string filename, ofstream* write, int ADDR_LSB_BITS, int BR_HIST_SFT_REG_WIDTH, int NUM_NEURON, int STCNTR_WIDTH);

int main(int argc, char* argv[]) {

	int budget[6] = {8*1024+64, 16*1024+128, 32*1024+256, 64*1024+512, 128*1024+1024, 1024*1024+4*1024};
	int ADDR_LSB_BITS_ARR[6] = {9,10,11,12,13,15};
	int BR_HIST_SFT_REG_WIDTH_ARR[6]= {8,10,11,12,13,17};
	int STCNTR_WIDTH=8;
	int NUM_NEURON_ARR[6] = {56,70,108,160,226,3418};

	const string files[4] = {"DIST-FP-1", "DIST-INT-1", "DIST-MM-1", "DIST-SERV-1"};
	ofstream output_file;
	output_file.open("results_combined_neural.csv");
	output_file << "COST" << "," << "COST in K" << ","<< "ADDR_LSB_BITS" << "," << "BR_HIST_SFT_REG_WIDTH" << ","<<"NUM_NEURON"<< "," << "STCNTR_WIDTH";
	for (int f = 0; f < 4; ++f) {
		output_file <<","<< files[f];
	}
	output_file << endl;

	for (int b = 0; b < 6; ++b) {
		int ADDR_LSB_BITS = ADDR_LSB_BITS_ARR[b];
		int BR_HIST_SFT_REG_WIDTH  = BR_HIST_SFT_REG_WIDTH_ARR[b];
		int NUM_NEURON = NUM_NEURON_ARR[b];

		Predictor p(NULL, ADDR_LSB_BITS, BR_HIST_SFT_REG_WIDTH, NUM_NEURON, STCNTR_WIDTH);
		int cost = p.getcost();
		if(cost<=budget[b] && cost>budget[b]/4 && (b==0 || cost>budget[b-1])){
			output_file << p.getcost() << "," << p.getcost()/1024 << ","<< ADDR_LSB_BITS << "," << BR_HIST_SFT_REG_WIDTH << "," <<NUM_NEURON<< "," << STCNTR_WIDTH;
			for (int i = 0; i < 4; ++i) {
				main_predictor(files[i], &output_file, ADDR_LSB_BITS, BR_HIST_SFT_REG_WIDTH, NUM_NEURON, STCNTR_WIDTH);
			}
			output_file << endl;
		}
	}




	output_file.close();

}
int main_analysis(int argc, char* argv[]) {

	int budget[6] = {8*1024+64, 16*1024+128, 32*1024+256, 64*1024+512, 128*1024+1024, 1024*1024+4*1024};
	int ADDR_LSB_BITS=5;
	int BR_HIST_SFT_REG_WIDTH=15;
	int STCNTR_WIDTH=8;
	int NUM_NEURON = 25;
	const string files[4] = {"DIST-FP-1", "DIST-INT-1", "DIST-MM-1", "DIST-SERV-1"};
	ofstream* output_file[6];
	for (int i = 0; i < 6; ++i) {
		output_file[i] = new ofstream;
		std::string s; std::stringstream ss;
		ss << budget[i];
		string outfile = "budget_"+ss.str()+"_results.csv";
		output_file[i]->open(outfile.c_str());
	}

	Params* budg_params[6];
	budg_params[0] = new Params(new Range(7,12), new Range(7,12), new Range(1,256), new Range(8,8));
	budg_params[1] = new Params(new Range(8,14), new Range(8,13), new Range(1,256), new Range(8,8));
	budg_params[2] = new Params(new Range(9,15), new Range(9,14), new Range(1,256), new Range(8,8));
	budg_params[3] = new Params(new Range(10,16), new Range(10,15), new Range(1,256), new Range(8,8));
	budg_params[4] = new Params(new Range(11,17), new Range(11,16), new Range(1,256), new Range(8,8));
	budg_params[5] = new Params(new Range(12,18), new Range(12,17), new Range(1,256), new Range(8,8));

	/*BranchHistory b(true, 5);
	b.print();
	b.update(true);
	b.print();
	b.update(false);
	b.print();
	b.update(true);
	b.print();
	b.update(true);
	b.print();
	b.update(true);
	b.print();
	return 0;*/
	for (int i = 0; i < 6; ++i) {
		*output_file[i] << "COST" << "," << "COST in K" << ","<< "ADDR_LSB_BITS" << "," << "BR_HIST_SFT_REG_WIDTH" << ","<<"NUM_NEURON"<< "," << "STCNTR_WIDTH";
		for (int f = 0; f < 4; ++f) {
			*output_file[i] <<","<< files[f];
		}
		*output_file[i] << endl;
	}
	omp_set_num_threads(6);
#pragma omp parallel for
	for (int b = 2; b < 3; ++b) {
		Params* p = budg_params[b];
		for (int ADDR_LSB_BITS = p->addr_bits->min; ADDR_LSB_BITS <= p->addr_bits->max; ++ADDR_LSB_BITS) {
			printf("b=%d ADDR_LSB_BITS=%d\n",b, ADDR_LSB_BITS);
			for (int BR_HIST_SFT_REG_WIDTH = p->hist_bits->min; BR_HIST_SFT_REG_WIDTH <= p->hist_bits->max ; ++BR_HIST_SFT_REG_WIDTH) {
				printf("b=%d BR_HIST_SFT_REG_WIDTH=%d\n",b, BR_HIST_SFT_REG_WIDTH);
				for (int STCNTR_WIDTH = p->sat_cntr->min; STCNTR_WIDTH <= p->sat_cntr->max; ++STCNTR_WIDTH) {
					Predictor t(NULL, ADDR_LSB_BITS, BR_HIST_SFT_REG_WIDTH, 1, STCNTR_WIDTH);
					t.init_predictor();
					int NUM_NEURON = t.getNumNeurons(budget[b]);
					if(NUM_NEURON>0){
						Predictor p(NULL, ADDR_LSB_BITS, BR_HIST_SFT_REG_WIDTH, NUM_NEURON, STCNTR_WIDTH);
						int cost = p.getcost();
						if(cost<=budget[b] && cost>budget[b]/4 && (b==0 || cost>budget[b-1])){
							*output_file[b] << p.getcost() << "," << p.getcost()/1024 << ","<< ADDR_LSB_BITS << "," << BR_HIST_SFT_REG_WIDTH << "," <<NUM_NEURON<< "," << STCNTR_WIDTH;
							for (int i = 0; i < 4; ++i) {
								main_predictor(files[i], output_file[b], ADDR_LSB_BITS, BR_HIST_SFT_REG_WIDTH, NUM_NEURON, STCNTR_WIDTH);
							}
							*output_file[b] << endl;
						}
					}
				}
			}
		}
	}
	/*STCNTR_WIDTH = 8;
	for (int ADDR_LSB_BITS = 8; ADDR_LSB_BITS <= 19; ++ADDR_LSB_BITS) {
		printf("b=%d ADDR_LSB_BITS=%d\n",0, ADDR_LSB_BITS);
		for (int BR_HIST_SFT_REG_WIDTH = 19; BR_HIST_SFT_REG_WIDTH <= 19 ; ++BR_HIST_SFT_REG_WIDTH) {
			printf("b=%d BR_HIST_SFT_REG_WIDTH=%d\n",0, BR_HIST_SFT_REG_WIDTH);
			for (int NUM_NEURON = 2; NUM_NEURON <= 2; ++NUM_NEURON) {
				Predictor p(NULL, ADDR_LSB_BITS, BR_HIST_SFT_REG_WIDTH, NUM_NEURON, STCNTR_WIDTH);
				int cost = p.getcost();
	 *output_file[0] << p.getcost() << "," << p.getcost()/1024 << ","<< ADDR_LSB_BITS << "," << BR_HIST_SFT_REG_WIDTH << "," <<NUM_NEURON<< "," << STCNTR_WIDTH;
				for (int i = 0; i < 4; ++i) {
					main_predictor(files[i], output_file[0], ADDR_LSB_BITS, BR_HIST_SFT_REG_WIDTH, NUM_NEURON, STCNTR_WIDTH);
				}
	 *output_file[0] << endl;
			}
		}
	}
	for (int ADDR_LSB_BITS = 19; ADDR_LSB_BITS <= 19; ++ADDR_LSB_BITS) {
		printf("b=%d ADDR_LSB_BITS=%d\n",0, ADDR_LSB_BITS);
		for (int BR_HIST_SFT_REG_WIDTH = 8; BR_HIST_SFT_REG_WIDTH <= 19 ; ++BR_HIST_SFT_REG_WIDTH) {
			printf("b=%d BR_HIST_SFT_REG_WIDTH=%d\n",0, BR_HIST_SFT_REG_WIDTH);
			for (int NUM_NEURON = 2; NUM_NEURON <= 2; ++NUM_NEURON) {
				Predictor p(NULL, ADDR_LSB_BITS, BR_HIST_SFT_REG_WIDTH, NUM_NEURON, STCNTR_WIDTH);
				int cost = p.getcost();
	 *output_file[0] << p.getcost() << "," << p.getcost()/1024 << ","<< ADDR_LSB_BITS << "," << BR_HIST_SFT_REG_WIDTH << "," <<NUM_NEURON<< "," << STCNTR_WIDTH;
				for (int i = 0; i < 4; ++i) {
					main_predictor(files[i], output_file[0], ADDR_LSB_BITS, BR_HIST_SFT_REG_WIDTH, NUM_NEURON, STCNTR_WIDTH);
				}
	 *output_file[0] << endl;
			}
		}
	}*/

	/*for (int i = 0; i < 1; ++i) {
		main_predictor(files[0], output_file[0], 5, 10, 2, 2);
	}*/
	for (int i = 0; i < 4; ++i) {
		output_file[i]->close();
		free(output_file[i]);
	}
}

int main_predictor(string filename, ofstream* write, int ADDR_LSB_BITS, int BR_HIST_SFT_REG_WIDTH, int NUM_NEURON, int STCNTR_WIDTH){
	//printf("-----------------------------\nStats for %s\n-----------------------------\n",filename.c_str());
	int mis_preds = 0;
	int num_branches = 0;
	uint32_t pc = 0;
	bool outcome = false;

	Traceread trace;
	trace.setup_trace(filename.c_str());
	if(trace.stream==NULL)
		printf ("Could not read intput file\n");

	Predictor predictor(&trace, ADDR_LSB_BITS, BR_HIST_SFT_REG_WIDTH, NUM_NEURON, STCNTR_WIDTH);
	// Initialize the predictor
	predictor.init_predictor();


	// Read the number of instructions from the trace
	uint32_t stat_num_insts = 0;
	if (fread (&stat_num_insts, sizeof (uint32_t), 1, trace.stream) != 1) {
		printf ("Could not read intput file\n");
		return 1;
	}
	stat_num_insts = ntohl (stat_num_insts);

	// Read each branch from the trace
	while (trace.read_branch (&pc, &outcome)) {

		pc = ntohl (pc);

		num_branches ++;

		// Make a prediction and compare with actual outcome
		if (predictor.make_prediction (pc) != outcome)
			mis_preds ++;

		// Train the predictor
		predictor.train_predictor (pc, outcome);
	}

	//predictor.print_cost();

	// Print out the mispredict statistics
	//printf ("Branches\t\t%8d\n", num_branches);
	//printf ("Incorrect\t\t%8d\n", mis_preds);
	float mis_pred_rate = (float)mis_preds / float(stat_num_insts / 1000);
	//printf ("1000*wrong_cc_predicts/total insts 1000 * %8d / %8d = %7.3f\n", mis_preds, stat_num_insts, mis_pred_rate);
	*write <<","<< mis_pred_rate;
	trace.close_trace ();

	return 0;
}
