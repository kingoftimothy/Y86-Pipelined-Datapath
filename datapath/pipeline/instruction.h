#ifndef _INSTRUCTION_H_
#define _INSTRUCTION_H_

#include "isa.h"
#include "registers.h"
#include "util.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// Represents an individual Y86-64 instruction instance including its name and
// operands.

// Constants indicating the maximum number of registers an instruction may
// read and write.  This includes registers explicitly mentioned in assembly
// instructions and registers not mentioned in the assembly but implicitly
// accessed (e.g., stack pointer register).
#define MAX_SRC_REGS 3
#define MAX_DEST_REGS 2

// Constant representing the distinct number of fields that may appear
// in an assembly instruction.  (e.g., addq 10(%rdi), %rax has 3 fields:
// 10, %rdi, and %rax.
#define MAX_ASM_FIELDS 3

// The different types of register accesses
typedef enum { REG_READ, REG_WRITE, REG_RDWR} RegAccessType;

// Representation of each Y86-64 instruction.  In addition to the
// instruction name, contains information about the instruction's
// address and its source and destination operands from the assembly
// instruction.  Also contains lists of all registers read and written
// (explicit and implicit) by instruction.
typedef struct {
  // Portions of assembly instruction
  char *addr;
  char *name;
  int numOperands;
  char *operands[MAX_ASM_FIELDS];

  // Actual registers used in instruction (explicit and implicit).
  int numSrcRegisters;
  RegID srcRegisters[MAX_SRC_REGS];
  int numDestRegisters;
  RegID destRegisters[MAX_DEST_REGS];
} Instruction;


// Returns a new string containing the Y86-64 assembly representation of
// a string.  The string will contain the instruction address, the
// instruction name, and the operands in order.  
char *getInstructionAssembly(Instruction *instr);

// Dynamically allocates the memory needed to store the specified fields
// in the specified instruction instance and initializes the fields in
// the specified instruction instance with the values specified as
// arguments.
void allocateSpace(Instruction * instr, char *addr, char *name, char *op1,
		   char *op2, char *op3);

// Frees the dynamically allocated memory used to store the fields in
// the specified instruction instance.
void freeSpace(Instruction *instr);

// Parses the operands specified as part of the assembly instruction to
// pull out the source and destination registers.  Creates lists of
// all registers read and written (implicit and explicit) in the
// specified Instruction instance.
void parseOperands(Instruction *instr);

// Returns the RegID in the specified instruction for the specified
// operand number. Also sets access type to indicate how the register
// is used (read, written, both).  Returns REG_NUM if operandNum is
// not valid or instruction is NULL.  Access type is undefined if function
// returns REG_NUM.
RegID getRegInOperand(Instruction *instr, int operandNum,
		      RegAccessType *accessType);

// Returns the number of registers that will be read by instruction.
// Copies into specified array the source registers.  Array must be of
// size MAX_SRC_REGS.  
int getSrcRegisters(Instruction *instr, RegID *srcRegs);

// Returns the number of registers that will be written by
// instruction.  Copies into specified array the source registers.
// Array must be of size MAX_DEST_REGS.
int getDestRegisters(Instruction *instr, RegID *srcRegs);


#endif
