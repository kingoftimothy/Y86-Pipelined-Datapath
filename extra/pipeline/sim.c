#include <stdio.h>
#include <stdlib.h>
#include "isa.h"
#include "registers.h"
#include "parser.h"
#include "sim.h"
#include "instruction.h"

// Prints usage information
void printUsage()
{
  printf("Usage: sim <tracefile> <# instructions>\n");
}

// Given a Y86-64 tracefile and the number of instructions to simulate in
// the trace file, simulates executing those instructions on an ideal
// pipeline, a stalling pipeline, and a data forwarding pipeline and prints
// out the completion times of those instructions on each pipeline.
int main(int argc, char *argv[])
{
  if(argc < 3){
    printUsage();
    exit(EXIT_FAILURE);
  }

  // Make sure the number of instructions specified is a positive integer
  int numInstrs = atoi(argv[2]);
  if(numInstrs < 1){
    printUsage();
    exit(EXIT_FAILURE);
  }

  // Get the set of instructions from the trace file
  Instruction *instrs = readInstructions(argv[1], numInstrs);


  // Execute and print the results of the ideal pipeline
  InstructionCompletion * noHazardTimes = executeNoHazardPipeline(instrs,
								  numInstrs);
  printCompletionTimes("No hazards", noHazardTimes, numInstrs);
  free(noHazardTimes);

  // Execute and print the results of the data forwarding  pipeline  
  InstructionCompletion * forwardTimes = executeForwardPipeline(instrs,
							    numInstrs);
  printCompletionTimes("\nForward on hazards", forwardTimes, numInstrs);
  free(forwardTimes);  

  // Need to free instructions
  freeInstructions(instrs, numInstrs);

}

// Given an array of instructions, execute the specified number of
// instructions on the pipeline and return an array of InstructionCompletion
// instances, one for each of the numInstrs that indicates the instruction's
// completion time.  This pipeline is ideal in that no instructions stall.
InstructionCompletion * executeNoHazardPipeline(Instruction *instrs,
						int numInstrs)
{
  // Create completion time array
  InstructionCompletion *completeTimes =
    (InstructionCompletion*)malloc(numInstrs * sizeof(InstructionCompletion));

  // Initialize an empty pipeline of NUM_STAGES.  Each pipeline stage
  // stores a pointer to the Instruction in that stage or NULL if no
  // instruction is currently in that stage.
  Instruction *pipeline[NUM_STAGES];
  for(int i = 0; i < NUM_STAGES; i++){
    pipeline[i] = NULL;
  }

  // Simulate the pipeline
  int time = 0;
  int completedCount = 0;
  int enteredCount = 0;
  
  // While not all instructions have completed execution
  while(enteredCount < numInstrs || !isPipelineEmpty(pipeline)){
    
    // Move instructions already in the pipeline one stage forward starting at
    // end of pipeline and moving forward to first pipeline stage
    for(int i = NUM_STAGES-1; i >= 0; i--){      
      if(pipeline[i] != NULL){
	// If the pipeline stage is not empty
	
	if(i == PIPE_WRITE_REG_STAGE){
	  // If the pipeline stage is the last stage where we write to
	  // registers, create an InstructionCompletion entry for the
	  // instruction
	  completeTimes[completedCount].instr = pipeline[i];
	  completeTimes[completedCount].completionTime = time;
	  completedCount++;
	  pipeline[i] = NULL;
	}
	else{
	  // If the pipeline stage is not the last stage, just advance
	  // the instruction one stage.  Since we don't recognize hazards
	  // and consequently don't stall instructions, there is no reason
	  // to check if the next pipeline stage is unoccupied before
	  // advancing.
	  pipeline[i+1] = pipeline[i];
	  pipeline[i] = NULL;
	}
      }
    }
    
    if(enteredCount < numInstrs){
      // This handles the insertion of instructions into the first
      // stage of the pipeline.  If there are instructions that
      // haven't entered the pipeline yet, insert the next instruction
      // into the pipeline's first stage.
      pipeline[PIPE_ENTER_STAGE] =  &instrs[enteredCount];
      enteredCount++;
    }

    // Advance time
    time++;
  }

  // Return the array of InstructionCompletion instances
  return completeTimes;
}

// Given an array of instructions, execute the specified number of
// instructions on the pipeline and return an array of
// InstructionCompletion instances, one for each of the numInstrs that
// indicates the instruction's completion time.  This pipeline stalls
// instructions waiting for their source operand registers to be
// written by an instruction already in the pipeline until that
// instruction has produced the value and can forward it to the
// instruction needing it's value.  It also stalls on control
// instructions until the target destination is know.
InstructionCompletion * executeForwardPipeline(Instruction *instrs,
					       int numInstrs)
{
  // Create completion time array
  InstructionCompletion *completeTimes =
    (InstructionCompletion*)malloc(numInstrs * sizeof(InstructionCompletion));

  // Initialize an empty pipeline of NUM_STAGES.  Each pipeline stage
  // stores a pointer to the Instruction in that stage or NULL if no
  // instruction is currently in that stage.
  Instruction *pipeline[NUM_STAGES];
  for(int i = 0; i < NUM_STAGES; i++){
    pipeline[i] = NULL;
  }

  // Simulate the pipeline
  int time = 0;
  int completedCount = 0;
  int enteredCount = 0;
  
  // While not all instructions have completed execution
  while(enteredCount < numInstrs || !isPipelineEmpty(pipeline)){
    
    // Move instructions already in the pipeline one stage forward starting at
    // end of pipeline and moving forward to first pipeline stage
    for(int i = NUM_STAGES-1; i >= 0; i--){      
      if(pipeline[i] != NULL){
	// If the pipeline stage is not empty
	
	if(i == PIPE_WRITE_REG_STAGE){
	  // If the pipeline stage is the last stage where we write to
	  // registers, create an InstructionCompletion entry for the
	  // instruction
	  completeTimes[completedCount].instr = pipeline[i];
	  completeTimes[completedCount].completionTime = time;
	  completedCount++;
	  pipeline[i] = NULL;
	}
	else{
	  // If the pipeline stage is not the last stage, check if it is in
	  // the execute stage and has a RAW dependence and if we can use
	  // data forwarding. If we can, then advance the instruction one
	  // stage. If we cannot, then stall for one stage (do nothing).
	  if (pipeline[i+1] == NULL) {  
	    if(i == PIPE_READ_FORWARD_REG_STAGE){
	      if(!needDataForwarding(pipeline, i)) {
	        // advance the instruction
	        pipeline[i+1] = pipeline[i];
	        pipeline[i] = NULL;
	      } else {
	    	// Need data forwarding, stall for one stage. 
	      }
	    } else {
	      // advance the instruction
	      pipeline[i+1] = pipeline[i];
	      pipeline[i] = NULL;
	    }
	  }
        }
      }
    }
    
    if(enteredCount < numInstrs && !hasControlDependence(pipeline)){
      // This handles the insertion of instructions into the first
      // stage of the pipeline.  If there are instructions that
      // haven't entered the pipeline yet, insert the next instruction
      // into the pipeline's first stage.
      if(pipeline[PIPE_ENTER_STAGE] == NULL) {
        pipeline[PIPE_ENTER_STAGE] =  &instrs[enteredCount];
        enteredCount++;
      }
    } else {
      // Has control dependence, stall for one stage.
    }

    // Advance time
    time++;
  }
  return completeTimes;
}

// Returns 1 if all pipeline stages are empty.  Returns 0 otherwise.
int isPipelineEmpty(Instruction *pipeline[])
{
  for(int i = 0; i < NUM_STAGES; i++){
    if(pipeline[i] != NULL){
      return 0;
    }
  }
  return 1;
}

// For a specified pipeline, print out the specified number of instructions
// and their completion times to stdout
void printCompletionTimes(char *pipelineName,
			  InstructionCompletion *instrTimes, int numInstrs)
{
  if(instrTimes == NULL)
    return;
  
  printf("\n%s:\n", pipelineName);

  printf("Instr# \t Addr \t Instruction \t\t\t\tCompletion Time\n");  
  printf("------ \t ---- \t ----------- \t\t\t\t---------------\n");
  for(int i = 0; i < numInstrs; i++){
    char buffer[40];
    char *asmPtr = getInstructionAssembly(instrTimes[i].instr);
    padString(buffer, asmPtr, 40);

    printf("%d:   \t %s \t%d\n", (i+1),   
	   buffer,
	   instrTimes[i].completionTime);
    free(asmPtr);
  }
      
}

// For a specified instruction, check if it needs data forwarding
int needDataForwarding(Instruction *pipeline[], int index){
  // two cases:
  // 1. If dest regs read from memory, prev instr must be past
  //    memory stage
  // 2. If dest regs do not read from memory, prev instr must be
  //    past execute stage
    
  // specified instruction
  // get source registers of specified instruction
  RegID srcRegs[MAX_SRC_REGS];
  int numSrcRegs = getSrcRegisters(pipeline[index], srcRegs);
  
  // Loop through previous instructions and check if any of their
  // destination registers are the same of the source registers of
  // the specified instruction.
  for(int i = 0; i < numSrcRegs; i++){
    // current srcReg to check
    RegID srcReg = srcRegs[i];

    // check every stage before the memory stage
    for(int j = index + 1; j < PIPE_MEMORY_PRODUCE_STAGE ; j++){
      if(pipeline[j] != NULL) {
        // previous instruction to check
        // get destination registers of specified instruction
        RegID destRegs[MAX_DEST_REGS];
        int numDestReg = getDestRegisters(pipeline[j], destRegs);
      
        // loop through destination registers and check if any are the same as
        // specified instruction's source registers
        for(int k = 0; k < numDestReg; k++){
	  // current destReg to check
	  RegID destReg = destRegs[k];

	  // check if source reg and dest reg are the same, there is a dependency
	  if(srcReg == destReg){
	    // if instr reads from memory, need data forwarding
	    if(getDestOperandType(getRegisterName(destReg)) == OP_MEMORY){
	      return 1;
	    }
	    // if instr does not read from memory, but is before execute
	    else if(j < PIPE_ALU_PRODUCE_STAGE){
	      return 1;
	    }
	    // if instr does not read from memory and is past execute
	    else{
	      return 0;
	    }
	  }

	  // if there is no dependency
	  else{
	    return 0;
	  }
        }
      }
    }
  }
  // no dependence
  return 0;
}
	
// Check if there is currently a control dependence by checking if any
// instruction before the memory stage is a control instruction.
int hasControlDependence(Instruction *pipeline[]) {
  for(int i = PIPE_ENTER_STAGE; i <= PIPE_KNOW_NEXTPC; i++){

    Instruction *instr = pipeline[i];
    // check if instruction is a control instruction
    if(instr != NULL){
      
      if(isControlInstruction(instr->name)){
	// there is a control dependency
	return 1;
      }
    }
  }
  // no control dependency
  return 0;
}

