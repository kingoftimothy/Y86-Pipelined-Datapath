#include "parser.h"

// Given a specified Y86-64 trace file filename and the number of
// instructions in that file, returns an array of Instruction instances,
// with one Instruction instance per line in the trace file, specified
// in the same order as the instructions appear in the trace file.
// Function will exit if unable to read trace file, if an instruction
// in the file is not valid, or if the number of instructions in the
// trace file is fewer than specified.
Instruction *readInstructions(char *filename, int numInstrs)
{
  // Open file
  FILE *file = fopen(filename, "r");
  if(file == NULL){
    fprintf(stderr, "Unable to open input file %s\n", filename);
    exit(EXIT_FAILURE);
  }

  // Allocate array of Instructions
  Instruction * instrs = (Instruction*)malloc(numInstrs * sizeof(Instruction));
  int instrCount = 0;

  // Read in file line by line and initialize Instruction instances
  while(!feof(file) && instrCount < numInstrs){
    char line[MAX_LINE_LENGTH];

    // Read line from file and check that it is not empty
    int length = readLine(file, line);
    if(length <= 0)
      continue;

    // Parse line from file into its parts and store in Instruction instance
    if(!populateInstruction( &instrs[instrCount], line)){
      // Exit program if instruction is invalid
      fprintf(stderr, "Unable to parse input line %d: %s\n", instrCount, line);
      exit(EXIT_FAILURE);
    }
    instrCount++;
  }

  fclose(file);

  if(instrCount != numInstrs){
    // If the number of instructions in the file is less than specified, exit.
    fprintf(stderr, "Input file has fewer than %d instructons.\n", numInstrs);
    exit(EXIT_FAILURE);
  }
  
  return instrs;
}

// Parses the specified line and stores values from the line into the
// appropriate fields in the Instruction instance.  Returns 1 if
// line was formatted properly and returns 0 otherwise.
int populateInstruction(Instruction *instr, char *line)
{
  char address[MAX_LINE_LENGTH];
  char name[MAX_LINE_LENGTH];
  char operand[MAX_NUM_OPERANDS][MAX_LINE_LENGTH];

  // Read the fields from the input file
  int num = sscanf(line, "PC: %s %s %s %s %s", address, name, operand[0],
		   operand[1], operand[2]);

  if(! isValidInstructionName(name)){
    return 0;
  }

  // Initialize the fields in Instruction instance based on format of
  // Y86-64 instruction.
  // Does some error checking on the number of fields.  Would need to
  // be rewritten if ISA changed.
  InstrGroup group = getInstructionGroup(name);
  switch(group){
  case I_HALT:
  case I_NOP:
    if(num != 2)
      return 0;
    allocateSpace(instr, address, name, NULL, NULL, NULL);
    break;
  case I_RRMOVQ:
  case I_IRMOVQ:
  case I_ALU:
    if(num != 4)
      return 0;
    allocateSpace(instr, address, name, operand[0], operand[1], NULL);
    break;
  case I_MRMOVQ:
    if(num < 4)
      return 0;    
    if(num == 4)
      // Insert displacement of 0 for memory operand      
      allocateSpace(instr, address, name, "0", operand[0], operand[1]);
    else
      allocateSpace(instr, address, name, operand[0], operand[1], operand[2]);
    break;
  case I_RMMOVQ:
    if(num < 4)
      return 0;    
    if(num == 4)
      // Insert displacement of 0 for memory operand
      allocateSpace(instr, address, name, operand[0], "0", operand[1]);
    else
      allocateSpace(instr, address, name, operand[0], operand[1], operand[2]);
    break;
  case I_JMP:
  case I_CALL:
  case I_PUSHQ:
  case I_POPQ:
    if(num != 3)
      return 0;
    allocateSpace(instr, address, name, operand[0], NULL, NULL);
    break;
  case I_RET:
    if(num != 2)
      return 0;
    allocateSpace(instr, address, name, NULL, NULL, NULL);
    break;
  default:
    return 0;
  }

  // Set up the lists of registers (implicit and explicit) read and written
  // in this Instruction.
  parseOperands(instr);
  
  return 1;
}

// Frees the dynamically allocated memory associated the array of
// specified Instructions
void freeInstructions(Instruction *instrs, int numInstructions)
{
  if(instrs == NULL)
    return;

  // Free the memory allocated to store fields of each instruction
  for(int i = 0; i < numInstructions; i++){
    freeSpace(&instrs[i]);
  }

  // Free array
  free(instrs);
}


// Reads in a line from the specified file into the specified line.
// Returns the number of characters read in and stored in line.
int readLine(FILE *file, char *line)
{
  int count = 0;
  char c = getc(file);

  while( ( c != '\n') && (c != EOF) ){
    line[count] = c;
    count++;
    c = getc(file);
  }
  line[count] = '\0';
  return count;
}
