#ifndef _UTIL_H_
#define _UTIL_H_

#include <string.h>

// Maximum length of any input line.  Defined here as it will be used in
// multiple code files.
#define MAX_LINE_LENGTH 80

// Maximum length of any assembly instructon name. 
#define MAX_NAME_LENGTH 10

// Given a source and destination string, copies the source string
// into the destination string and inserts extra space characters (' ')
// to make the destination string to have the specified length.
void padString(char *dest, char *src, int length);

#endif
