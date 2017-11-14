//============================================================================
// Name        : CSE-240A-project.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

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
int main_predictor(string filename, ofstream* write, int ADDR_LSB_BITS, int BR_HIST_SFT_REG_WIDTH, int STCNTR_WIDTH);

int main(int argc, char* argv[]) {

	int budget[6] = {8*1024+64, 16*1024+128, 32*1024+256, 64*1024+512, 128*1024+1024, 1024*1024+4*1024};
	int ADDR_LSB_BITS_ARR[6] = {12,13,14,15,16,19};

	int BR_HIST_SFT_REG_WIDTH_ARR[6] = {12,13,14,15,16,19};

	int STCNTR_WIDTH=2;
	const string files[4] = {"DIST-FP-1", "DIST-INT-1", "DIST-MM-1", "DIST-SERV-1"};
	ofstream output_file;
	output_file.open("results_combined_gshare.csv");
	output_file <<"BUDGET"<<","<< "COST" << "," << "COST in K" << ","<< "ADDR_LSB_BITS" << "," << "BR_HIST_SFT_REG_WIDTH" << "," << "STCNTR_WIDTH";
	for (int f = 0; f < 4; ++f) {
		output_file <<","<< files[f];
	}
	output_file << endl;

	for (int b = 0; b < 6; ++b) {
		int ADDR_LSB_BITS = ADDR_LSB_BITS_ARR[b];
		int BR_HIST_SFT_REG_WIDTH = BR_HIST_SFT_REG_WIDTH_ARR[b];
		Predictor p(NULL, ADDR_LSB_BITS, BR_HIST_SFT_REG_WIDTH, STCNTR_WIDTH);
		output_file << b << ","<< p.getcost() << "," << p.getcost()/1024 << ","<< ADDR_LSB_BITS << "," << BR_HIST_SFT_REG_WIDTH << "," << STCNTR_WIDTH;
		for (int i = 0; i < 4; ++i) {
			main_predictor(files[i], &output_file, ADDR_LSB_BITS, BR_HIST_SFT_REG_WIDTH, STCNTR_WIDTH);
		}
		output_file << endl;
	}

	output_file.close();

}
int main_analysis(int argc, char* argv[]) {

	int budget[6] = {8*1024+64, 16*1024+128, 32*1024+256, 64*1024+512, 128*1024+1024, 1024*1024+4*1024};
	int ADDR_LSB_BITS=5;
	int GL_HIST_SFT_REG_WIDTH=15;
	int STCNTR_WIDTH=2;
	const string files[4] = {"DIST-FP-1", "DIST-INT-1", "DIST-MM-1", "DIST-SERV-1"};
	ofstream* output_file[7];
	for (int i = 0; i < 6; ++i) {
		output_file[i] = new ofstream;
		std::string s; std::stringstream ss;
		ss << budget[i];
		string outfile = "budget_"+ss.str()+"_results.csv";
		output_file[i]->open(outfile.c_str());
	}
	output_file[6] = new ofstream;
	output_file[6]->open("results_combined_gshare.csv");

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
	for (int i = 0; i < 7; ++i) {
		*output_file[i] <<"BUDGET"<<"," << "COST" << "," << "COST in K" << ","<< "ADDR_LSB_BITS" << "," << "GL_HIST_SFT_REG_WIDTH" << "," << "STCNTR_WIDTH";
		for (int f = 0; f < 4; ++f) {
			*output_file[i] <<","<< files[f];
		}
		*output_file[i] << endl;
	}
	for (int b = 0; b < 6; ++b) {
		for (STCNTR_WIDTH = 2; STCNTR_WIDTH <= 2; ++STCNTR_WIDTH) {
			for (ADDR_LSB_BITS = 7; ADDR_LSB_BITS < 20; ++ADDR_LSB_BITS) {
				for (GL_HIST_SFT_REG_WIDTH = ADDR_LSB_BITS; GL_HIST_SFT_REG_WIDTH >= ADDR_LSB_BITS-2; --GL_HIST_SFT_REG_WIDTH) {
					Predictor p(NULL, ADDR_LSB_BITS, GL_HIST_SFT_REG_WIDTH, STCNTR_WIDTH);
					int cost = p.getcost();
					if(cost<=budget[b] && cost>((budget[b]/1024)*1024)/4 && (b==0 || cost>budget[b-1])){
						*output_file[6] << b << "," << p.getcost() << "," << p.getcost()/1024 << ","<< ADDR_LSB_BITS << "," << GL_HIST_SFT_REG_WIDTH << "," << STCNTR_WIDTH;
						for (int i = 0; i < 4; ++i) {
							main_predictor(files[i], output_file[6], ADDR_LSB_BITS, GL_HIST_SFT_REG_WIDTH, STCNTR_WIDTH);
						}
						*output_file[6] << endl;
					}
				}
			}
		}
	}
	/*
	Predictor p(NULL, 11, 11, 2);
	p.init_predictor();

	for (int i = 0; i < 15; ++i) {


		p.make_prediction(0xFFFF);
		p.train_predictor(0xFFFF, true);
	}

	p.make_prediction(0xFFFE);
	p.train_predictor(0xFFFE, true);
	p.make_prediction(0xFFFF);
	p.train_predictor(0xFFFF, true);
	p.make_prediction(0xFFFD);
	p.train_predictor(0xFFFD, true);*/
	/*for (int i = 0; i < 1; ++i) {
		main_predictor(files[i], output_file[6], 11, 10, 2);
	}*/
	for (int i = 0; i < 7; ++i) {
		output_file[i]->close();
		free(output_file[i]);
	}
}


int main_predictor(string filename, ofstream* write, int ADDR_LSB_BITS, int GL_HIST_SFT_REG_WIDTH, int STCNTR_WIDTH){
	//printf("-----------------------------\nStats for %s\n-----------------------------\n",filename.c_str());
	int mis_preds = 0;
	int num_branches = 0;
	uint32_t pc = 0;
	bool outcome = false;

	Traceread trace;
	trace.setup_trace(filename.c_str());
	if(trace.stream==NULL)
		printf ("Could not read intput file\n");

	Predictor predictor(&trace, ADDR_LSB_BITS, GL_HIST_SFT_REG_WIDTH, STCNTR_WIDTH);
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
