#include "registers.h"

// Array of Register state including register name and value.
// This array should not be accessed outside of this file.  Instead,
// the functions provided in this file should be used to gain access
// to the information stored in this array.
Register registerSet[] = 
{
  { "%rax", REG_RAX },
  { "%rcx", REG_RCX },
  { "%rdx", REG_RDX },
  { "%rbx", REG_RBX },
  { "%rsp", REG_RSP },
  { "%rbp", REG_RBP },
  { "%rsi", REG_RSI },
  { "%rdi", REG_RDI },
  { "%r8",  REG_R8 },
  { "%r9",  REG_R9 },
  { "%r10", REG_R10 },
  { "%r11", REG_R11 },
  { "%r12", REG_R12 },
  { "%r13", REG_R13 },
  { "%r14", REG_R14 },
  { "----", REG_NONE },  

};


// Returns a string representing the specified register name
char *getRegisterName(RegID id)
{
  if(id >= 0 && id < REG_NUM){
    return registerSet[id].name;
  }
  
  return NULL;
}

// Returns a number representing the specified string register's id 
RegID getRegisterId(char *regName)
{
  RegID result = REG_NUM;

  for(int i = 0; i < REG_NUM; i++){
    if(strcmp(regName, registerSet[i].name) == 0){
      return registerSet[i].id;
    }
  }
  
  return result;
}


