/*
 * Traceread.h
 *
 *  Created on: Nov 11, 2016
 *      Author: kdhiman
 */

#ifndef TRACEREAD_H_
#define TRACEREAD_H_
#include <stdio.h>

class Traceread {
public:
	FILE * stream;
	void setup_trace (const char * filename);
	bool read_branch (unsigned int * pc, bool * outcome);
	void close_trace ();
};

#endif /* TRACEREAD_H_ */
