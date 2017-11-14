/*
 * Traceread.cpp
 *
 *  Created on: Nov 11, 2016
 *      Author: kdhiman
 */

#include "Traceread.h"
#include <inttypes.h>


bool Traceread::read_branch (unsigned int * pc, bool * outcome){
	// read the pc
	if (fread (pc, sizeof (uint32_t), 1, stream) != 1)
		return false;

	// read the outcome
	uint8_t outcome_int;
	if (fread (&outcome_int, sizeof (uint8_t), 1, stream) != 1)
		return false;
	if (outcome_int == 0)
		*outcome = false;
	else
		*outcome = true;

	return true;

}

void Traceread::setup_trace (const char * filename)
{
	if (filename == NULL){
		stream = stdin;
	}
	else {
		stream = fopen (filename, "r");
	}
}

void Traceread::close_trace ()
{
	fclose (stream);
}
