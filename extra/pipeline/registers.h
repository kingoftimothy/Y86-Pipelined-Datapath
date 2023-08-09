#ifndef _REGISTERS_H_
#define _REGISTERS_H_

#include <stdlib.h>
#include <string.h>


// Representation of the 15 registers in the Y86-64 ISA.
// Registers can be named either with a C-style string (e.g. %rax)
// or with a number indicating the register's position in the register
// file.

// 15 valid registers in the Y86-64.
// Register 15 is used to indicate no register value.
typedef enum { REG_RAX, REG_RCX, REG_RDX, REG_RBX, REG_RSP, REG_RBP,
	       REG_RSI, REG_RDI, REG_R8, REG_R9, REG_R10, REG_R11,
	       REG_R12, REG_R13, REG_R14, REG_NONE, REG_NUM} RegID;


// Represents information stored for each Y86-64 register
typedef struct  {
  char *name;
  RegID id;
} Register;

// Returns a string representing the specified register name
char *getRegisterName(RegID id);

// Returns a number representing the specified string register's id 
RegID getRegisterId(char *regName);


#endif
