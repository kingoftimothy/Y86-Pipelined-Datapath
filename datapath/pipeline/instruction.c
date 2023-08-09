#include "instruction.h"
#include <assert.h>

// Returns a new string containing the Y86-64 assembly representation of
// a string.  The string will contain the instruction address, the
// instruction name, and the operands in order.  
char * getInstructionAssembly(Instruction *instr)
{
  char paddedName[MAX_NAME_LENGTH];
  // Used to add whitespace after instruction name before operands
  padString(paddedName, instr->name, MAX_NAME_LENGTH);
  
  char * str = (char*)malloc(sizeof(char) * MAX_LINE_LENGTH);
  
  if(getInstructionGroup(instr->name) == I_RMMOVQ){
    // Special format since memory operand will need to use parentheses
    // and displacement.
    if(instr->numOperands == MAX_ASM_FIELDS){
      sprintf(str, "%s\t %s %s, %s(%s)", instr->addr, paddedName,
	      instr->operands[0], instr->operands[1],
	     instr->operands[2]);
    }
  }
  else if(getInstructionGroup(instr->name) == I_MRMOVQ){
    // Special format since memory operand will need to use parentheses
    // and displacement.    
    if(instr->numOperands == MAX_ASM_FIELDS){
      sprintf(str, "%s\t %s %s(%s), %s", instr->addr, paddedName,
	      instr->operands[0], instr->operands[1],
	      instr->operands[2]);
    }
  }
  else{
    // Concatenate the operands together
    char operands[MAX_LINE_LENGTH];
    operands[0] = '\0';
    for(int i = 0; i < instr->numOperands; i++){
      if(i > 0)
	strcat(operands, ", ");
      strcat(operands, instr->operands[i]);
    }

    // Create the final string
    sprintf(str, "%s\t %s %s", instr->addr, paddedName, operands);
  }

  return str;
}

// Dynamically allocates the memory needed to store the specified fields
// in the specified instruction instance and initializes the fields in
// the specified instruction instance with the values specified as
// arguments.
void allocateSpace(Instruction * instr, char *addr, char *name, char *op1,
		   char *op2, char *op3)
{

  instr->addr = (char*)malloc(sizeof(char)*(strlen(addr)+1));
  strcpy(instr->addr, addr);

  instr->name = (char*)malloc(sizeof(char)*(strlen(name)+1));
  strcpy(instr->name, name);

  int numOperands = 0;
  if(op1 != NULL){
    instr->operands[0] = (char*)malloc(sizeof(char)*(strlen(op1)+1));
    strcpy(instr->operands[0], op1);
    numOperands++;
  }
  else
    instr->operands[0] = NULL;
  
  if(op2 != NULL){
    instr->operands[1] = (char*)malloc(sizeof(char)*(strlen(op2)+1));
    strcpy(instr->operands[1], op2);
    numOperands++;
  }
  else
    instr->operands[1] = NULL;
  
  if(op3 != NULL){
    instr->operands[2] = (char*)malloc(sizeof(char)*(strlen(op3)+1));
    strcpy(instr->operands[2], op3);
    numOperands++;
  }
  else
    instr->operands[2] = NULL;

  instr->numOperands = numOperands;
}

// Frees the dynamically allocated memory used to store the fields in
// the specified instruction instance.
void freeSpace(Instruction *instr)
{
  if(instr != NULL){
    if(instr->addr != NULL)
      free(instr->addr);

    if(instr->name != NULL)
      free(instr->name);

    for(int i = 0; i < MAX_ASM_FIELDS; i++){
      if(instr->operands[i] != NULL){
	free(instr->operands[i]);
      }
    }
  }
}

// Parses the operands specified as part of the assembly instruction to
// pull out the source and destination registers.
void parseOperands(Instruction *instr)
{
  instr->numSrcRegisters = instr->numDestRegisters = 0;

  int numAsmSrcOperands = getNumSourceOperands(instr->name);
  int numAsmDestOperands = getNumDestOperands(instr->name);
  
  for(int i = 0; i < numAsmSrcOperands; i++){
    // Examine source operands for the registers they read
    OperandType opType = getSourceOperandType(instr->name, i);
    
    if(opType != OP_MEMORY && opType != OP_REGISTER){
      // Only register and memory locations will cause registers to be read
      continue;
    }
       
    RegAccessType access = REG_READ;
    RegID regNum = getRegInOperand(instr, i, &access);

    if(regNum != REG_NUM){
      // There was a register in this operand
      
      if(access == REG_READ || access == REG_RDWR){
	instr->srcRegisters[instr->numSrcRegisters] = regNum;
	instr->numSrcRegisters++;
      }
      if(access == REG_RDWR){
	instr->destRegisters[instr->numDestRegisters] = regNum;
	instr->numDestRegisters++;	
      }
    }
  }

  if(numAsmDestOperands > 0 && numAsmSrcOperands != MAX_OPERANDS){
    // For any destination operands that aren't also source operands
    // Examine destination operands for the registers they read
    OperandType opType = getDestOperandType(instr->name);
    
    if(opType == OP_MEMORY || opType == OP_REGISTER){
      // Only register and memory locations will cause registers to be read
      RegAccessType access = REG_READ;
      RegID regNum = getRegInOperand(instr, getNumSourceOperands(instr->name),
				     &access);
      
      if(regNum != REG_NUM){
	// There was a register in this operand
	
	if(access == REG_READ){
	  instr->srcRegisters[instr->numSrcRegisters] = regNum;
	  instr->numSrcRegisters++;
	}
	if(access == REG_WRITE){
	  instr->destRegisters[instr->numDestRegisters] = regNum;
	  instr->numDestRegisters++;	
	}
      }
    }  
  }
  
  // Check if instruction implicitly access the stack and add RSP to
  // source and destination registers if it does.
  StackAccess stackAccess = getStackAccess(instr->name);
  if(stackAccess == STACK_ACCESS){
    instr->srcRegisters[instr->numSrcRegisters] = REG_RSP;
    instr->numSrcRegisters++;
    instr->destRegisters[instr->numDestRegisters] = REG_RSP;
    instr->numDestRegisters++;		
  }
  
}

// Returns the RegID in the specified instruction for the specified
// operand number. Also sets access type to indicate how the register
// is used (read, written, both).  Returns REG_NUM if operandNum is
// not valid or instruction is NULL.  Access type is undefined if function
// returns REG_NUM.
RegID getRegInOperand(Instruction *instr, int operandNum,
		      RegAccessType *accessType)
{
  int operandOffset = 0;

  if(instr == NULL)
    return REG_NUM;

  // Determine whether there is a displacement due to memory operand
  // and adjust operandOffset accordingly to find the register in
  // the operand array
  for(int i = 0; i < operandNum; i++){
    if(getSourceOperandType(instr->name, i) == OP_MEMORY){
      operandOffset += 2;
    }
    else{
      operandOffset += 1;
    }
  }

  if(operandNum < getNumSourceOperands(instr->name)){
    // If the operand should be a source operand according to the ISA,
    // return the appropriate register

    // Register will be read
    *accessType = REG_READ;
    
    if(getSourceOperandType(instr->name, operandNum) == OP_REGISTER){
      if(getNumSourceOperands(instr->name) == MAX_OPERANDS &&
	 operandNum == (MAX_OPERANDS-1) && 
	 getNumDestOperands(instr->name) > 0){
	// When second operand is both source and dest
	*accessType = REG_RDWR;
      }
      return getRegisterId(instr->operands[operandOffset]);
    }
    else if(getSourceOperandType(instr->name, operandNum) == OP_MEMORY){
      // This moves past the displacement in a memory instruction
      return getRegisterId(instr->operands[operandOffset+1]);
    }
    else{
      return REG_NUM;
    }
  }

  if(getNumSourceOperands(instr->name) != MAX_OPERANDS &&
     getNumDestOperands(instr->name) > 0){
    // operandNum is for a destination operand

    if(getDestOperandType(instr->name) == OP_REGISTER){
      // If the operand number is for a destination operand, return the
      // appropriate register
      
      // Registers that are destination operands are both read and written
      *accessType = REG_WRITE;
      return getRegisterId(instr->operands[operandOffset]);
    }
    else if(getDestOperandType(instr->name) == OP_MEMORY){
      // If the operand number is for a memory location, return the
      // register used to calculate the memory address
      
      // Registers used to calculate addresses are only read
      *accessType = REG_READ;
      // This moves past the displacement in a memory instruction      
      return getRegisterId(instr->operands[operandOffset+1]);
    }
    else{
      return REG_NUM;
    }
  }
  return REG_NUM;
}

// Returns the number of registers that will be read by instruction.
// Copies into specified array the source registers.  Array must be of
// size MAX_SRC_REGS.  
int getSrcRegisters(Instruction *instr, RegID *srcRegs)
{
  for(int i = 0; i < instr->numSrcRegisters; i++){
    srcRegs[i] = instr->srcRegisters[i];
  }
  return instr->numSrcRegisters;
}

// Returns the number of registers that will be written by
// instruction.  Copies into specified array the destination registers.
// Array must be of size MAX_DEST_REGS.
int getDestRegisters(Instruction *instr, RegID *destRegs)
{
  for(int i = 0; i < instr->numDestRegisters; i++){
    destRegs[i] = instr->destRegisters[i];
  }
  return instr->numDestRegisters;
}

  
