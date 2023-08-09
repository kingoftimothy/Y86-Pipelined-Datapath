#ifndef _ISA_H_
#define _ISA_H_

#include <stdlib.h>
#include <string.h>

// Representation of the Y86-64 instruction set.  Includes information
// about instructions including the instruction group they belong to
// and the number and type of operands required in each instruction.
// This information is the generic format used for each instruction
// and does not provide information about a specific instruction instance.

#define NUM_INSTRS 27
#define MAX_OPERANDS 2

// The different types of operands.
typedef enum { OP_IMMEDIATE, OP_MEMORY, OP_REGISTER, OP_ADDRESS,
	       NO_OPERAND} OperandType;

// The different instruction groups.  This corresponds to the first
// 4 bits used to encode a Y86-64 instruction.
typedef enum {I_HALT, I_NOP, I_RRMOVQ, I_IRMOVQ, I_RMMOVQ, I_MRMOVQ,
	      I_ALU, I_JMP, I_CALL, I_RET, I_PUSHQ, I_POPQ, 
	      NUM_INST_GROUPS} InstrGroup;

// This enum indictes whether an implicit stack register access occurs.  It is
// assumed that a stack access reads the stack register and updates it.
typedef enum {NO_STACK_ACCESS, STACK_ACCESS} StackAccess;

// Information used to represent each Y86-64 instruction format.
typedef struct {
  char *name;
  InstrGroup instGroup;
  OperandType operands[MAX_OPERANDS];
  int numSourceOps;
  int numDestOps;
  StackAccess stackAccess;
} InstrFormat;  

// Returns 1 if the specified string is the name for a valid instruction.
// Returns 0 otherwise.
int isValidInstructionName(char *name);

// Returns the InstrGroup associated with the specified instruction name.
// Returns NUM_INST_GROUPS when the specified string is not a valid
// instruction.
InstrGroup getInstructionGroup(char *name);

// Returns 1 if the instruction is considered a control instruction 
// (i.e., CALL, RET, JMP) and 0 otherwise.
int isControlInstruction(char *name);

// Returns the source operand type for a given instruction name and
// operand number.  Returns NO_OPERAND if the specified instruction name
// is not valid or if the specified operand number is a valid source
// operand number.
OperandType getSourceOperandType(char *name, int num);

// Returns the destination operand type for a given instruction name.
// Returns NO_OPERAND if specified instruction name is not valid or if
// instruction does not have any destination operands.
OperandType getDestOperandType(char *name);

// Returns the number of source operands for the specified instruction name.
// Returns 0 if the specified name is not valid.
int getNumSourceOperands(char *name);

// Returns the number of destination operands for the specified instruction
// name. Returns 0 if the specified name is not valid.
int getNumDestOperands(char *name);

// Returns whether or not the instruction accesses the stack register
StackAccess getStackAccess(char *name);


#endif
