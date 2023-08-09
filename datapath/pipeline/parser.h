#ifndef _PARSER_H_
#define _PARSER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "isa.h"
#include "instruction.h"
#include "util.h"

// Provides functionality for reading in a Y86-64 trace file and
// creating Instruction instances for each line in that file.

#define MAX_NUM_OPERANDS 3

// Given a specified Y86-64 trace file filename and the number of
// instructions in that file, returns an array of Instruction instances,
// with one Instruction instance per line in the trace file, specified
// in the same order as the instructions appear in the trace file.
// Function will exit if unable to read trace file, if an instruction
// in the file is not valid, or if the number of instructions in the
// trace file is fewer than specified.
Instruction *readInstructions(char *filename, int numInstrs);

// Parses the specified line and stores values from the line into the
// appropriate fields in the Instruction instance.  Returns 1 if
// line was formatted properly and returns 0 otherwise.
int populateInstruction(Instruction *instr, char *line);

// Frees the dynamically allocated memory associated the array of
// specified Instructions
void freeInstructions(Instruction *instrs, int numInstructions);

// Reads in a line from the specified file into the specified line.
// Returns the number of characters read in and stored in line.
int readLine(FILE *file, char *line);

#endif
