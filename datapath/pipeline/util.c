#include "util.h"

// Given a source and destination string, copies the source string
// into the destination string and inserts extra space characters (' ')
// to make the destination string to have the specified length.
void padString(char *dest, char *src, int length)
{
  strcpy(dest, src);
  for(int i = strlen(dest); i < length-1; i++){
    dest[i] = ' ';
  }
  dest[length-1] = '\0';

}
