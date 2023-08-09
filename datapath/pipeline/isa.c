#include "isa.h"

// Array of InstrFormat state for each Y86-64 instruction.  It includes
// information about the instruction name, instruction group, number of
// operands, the type of each operand, and the number of source and
// destination operands.
// This array should not be accessed outside of this file.  Instead,
// the functions provided in this file should be used to gain access
// to the information stored in this array.
InstrFormat instructionSet [] = 
{
 {"halt", I_HALT, {NO_OPERAND, NO_OPERAND}, 0, 0, NO_STACK_ACCESS},
 {"nop", I_NOP, {NO_OPERAND, NO_OPERAND}, 0, 0, NO_STACK_ACCESS},
 {"rrmovq", I_RRMOVQ, {OP_REGISTER, OP_REGISTER}, 1, 1, NO_STACK_ACCESS},
 {"cmovle", I_RRMOVQ, {OP_REGISTER, OP_REGISTER}, 1, 1, NO_STACK_ACCESS},
 {"cmovl", I_RRMOVQ, {OP_REGISTER, OP_REGISTER}, 1, 1, NO_STACK_ACCESS},
 {"cmove", I_RRMOVQ, {OP_REGISTER, OP_REGISTER},  1, 1, NO_STACK_ACCESS},
 {"cmovne", I_RRMOVQ, {OP_REGISTER, OP_REGISTER},  1, 1, NO_STACK_ACCESS},
 {"cmovge", I_RRMOVQ, {OP_REGISTER, OP_REGISTER},  1, 1, NO_STACK_ACCESS}, 
 {"cmovg", I_RRMOVQ, {OP_REGISTER, OP_REGISTER}, 1, 1, NO_STACK_ACCESS},
 {"irmovq", I_IRMOVQ, {OP_IMMEDIATE, OP_REGISTER}, 1, 1, NO_STACK_ACCESS},
 {"rmmovq", I_RMMOVQ, {OP_REGISTER, OP_MEMORY}, 1, 1, NO_STACK_ACCESS}, 
 {"mrmovq", I_MRMOVQ, {OP_MEMORY, OP_REGISTER},  1, 1, NO_STACK_ACCESS},
 {"addq", I_ALU, {OP_REGISTER, OP_REGISTER}, 2, 1, NO_STACK_ACCESS},
 {"subq", I_ALU, {OP_REGISTER, OP_REGISTER}, 2, 1, NO_STACK_ACCESS}, 
 {"andq", I_ALU, {OP_REGISTER, OP_REGISTER}, 2, 1, NO_STACK_ACCESS}, 
 {"xorq", I_ALU, {OP_REGISTER, OP_REGISTER}, 2, 1, NO_STACK_ACCESS},
 {"jmp", I_JMP, {OP_ADDRESS, NO_OPERAND}, 1, 0, NO_STACK_ACCESS},
 {"jle", I_JMP, {OP_ADDRESS, NO_OPERAND}, 1, 0, NO_STACK_ACCESS},
 {"jl", I_JMP, {OP_ADDRESS, NO_OPERAND}, 1, 0, NO_STACK_ACCESS},    
 {"je", I_JMP, {OP_ADDRESS, NO_OPERAND}, 1, 0, NO_STACK_ACCESS},   
 {"jne", I_JMP, {OP_ADDRESS, NO_OPERAND}, 1, 0, NO_STACK_ACCESS},   
 {"jge", I_JMP, {OP_ADDRESS, NO_OPERAND}, 1, 0, NO_STACK_ACCESS},   
 {"jg", I_JMP, {OP_ADDRESS, NO_OPERAND}, 1, 0, NO_STACK_ACCESS},
 {"call", I_CALL, {OP_ADDRESS, NO_OPERAND}, 1, 0, STACK_ACCESS},    
 {"ret", I_RET, {NO_OPERAND, NO_OPERAND}, 0, 0, STACK_ACCESS},
 {"pushq", I_PUSHQ, {OP_REGISTER, NO_OPERAND}, 1, 0, STACK_ACCESS},
 {"popq", I_POPQ, {NO_OPERAND, OP_REGISTER}, 0, 1, STACK_ACCESS},
 
};

// Returns 1 if the specified string is the name for a valid instruction.
// Returns 0 otherwise.
int isValidInstructionName(char *name)
{
  for(int i = 0; i < NUM_INSTRS; i++){
    if(strcmp(name, instructionSet[i].name) == 0){
      return 1;
    }
  }
  return 0;
}

// Returns the InstrGroup associated with the specified instruction name.
// Returns NUM_INST_GROUPS when the specified string is not a valid
// instruction.
InstrGroup getInstructionGroup(char *name)
{
  for(int i = 0; i < NUM_INSTRS; i++){
    if(strcmp(name, instructionSet[i].name) == 0){
      return instructionSet[i].instGroup;
    }
  }
  
  return NUM_INST_GROUPS;
}

// Returns 1 if the instruction is considered a control instruction 
// (i.e., CALL, RET, JMP) and 0 otherwise.
int isControlInstruction(char *name)
{
  InstrGroup instrType = getInstructionGroup(name);
  if(instrType == I_JMP || instrType == I_CALL || instrType == I_RET){
    return 1;
  }
  else
    return 0;
}

// Returns the source operand type for a given instruction name and
// operand number.  Returns NO_OPERAND if the specified instruction name
// is not valid or if the specified operand number is a valid source
// operand number.
OperandType getSourceOperandType(char *name, int num)
{

  for(int i = 0; i < NUM_INSTRS; i++){
    if(strcmp(name, instructionSet[i].name) == 0){
      if(num >= 0 && num < instructionSet[i].numSourceOps){
	return instructionSet[i].operands[num];
      }
    }
  }
  
  return NO_OPERAND;
}

// Returns the destination operand type for a given instruction name.
// Returns NO_OPERAND if specified instruction name is not valid or if
// instruction does not have any destination operands.
OperandType getDestOperandType(char *name)
{
  for(int i = 0; i < NUM_INSTRS; i++){
    if(strcmp(name, instructionSet[i].name) == 0){
      if(instructionSet[i].numDestOps > 0){
	if(instructionSet[i].numSourceOps == MAX_OPERANDS)
	  // This handles the case when the register is both source and dest
	  return instructionSet[i].operands[instructionSet[i].numSourceOps-1];
	else
	  return instructionSet[i].operands[instructionSet[i].numSourceOps]; 
      }
    }
  }
  return NO_OPERAND;
}

// Returns the number of source operands for the specified instruction name.
// Returns 0 if the specified name is not valid.
int getNumSourceOperands(char *name)
{
  for(int i = 0; i < NUM_INSTRS; i++){
    if(strcmp(name, instructionSet[i].name) == 0){
      return instructionSet[i].numSourceOps;
    }
  }
  return 0;
}

// Returns the number of destination operands for the specified instruction
// name. Returns 0 if the specified name is not valid.
int getNumDestOperands(char *name)
{
  for(int i = 0; i < NUM_INSTRS; i++){
    if(strcmp(name, instructionSet[i].name) == 0){
      return instructionSet[i].numDestOps;
    }
  }
  return 0;
}

// Returns 1 if the instruction accesses the stack register, otherwise 0.
StackAccess getStackAccess(char *name)
{
  for(int i = 0; i < NUM_INSTRS; i++){
    if(strcmp(name, instructionSet[i].name) == 0){
      return instructionSet[i].stackAccess;
    }
  }
  return 0;
  
}
