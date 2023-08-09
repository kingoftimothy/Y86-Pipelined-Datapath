#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 80

// Information about the 3 datapaths
enum Datapath { STALLING, FORWARDING, NUM_DATAPATHS};
char *DatapathNames[NUM_DATAPATHS] = {"Stalling", "Forwarding"};

// Represents a line in the input file
struct line {
  char text[MAX_LINE_LENGTH];  
  int time;
};

// Returns a dynamically allocated copy of the line with whitespace
// removed.
char * trim(char *line)
{
  int len = strlen(line);
  char *result = malloc(len+1);
  
  int firstNon = 0;
  
  for(int i = 0; i < len; i++){
    if(line[i] != ' ' && line[i] != '\t'){
      firstNon = i;
      break;
    }
  }

  int lastNon = 0;
  for(int i = len; i >= 0; i--){
    if(line[i] != ' ' && line[i] != '\t'){
      lastNon = i;
      break;
    }
  }

  strncpy(result, line+firstNon, (lastNon-firstNon+1)); 
  result[(lastNon-firstNon+1)] = '\0';
  return result;
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

// Splits the line into its two fields and stores into specified result
// struct
void splitLine(char *line, struct line* result)
{
  strcpy(result->text, line);
  char *str;
  char *last = NULL;

  str = strtok(line, "\t");
  
  while(str != NULL ) {
    last = str;
    str = strtok(NULL, "\t");
  }

  result->text[strlen(result->text) - strlen(last)] = '\0';
  
  // Put time in time field
  result->time = atoi(last);

}

// For the specified input file stream, reads in specified number of
// instructions and returns an array of line structs, one per input line.
struct line* readDatapathLines(FILE * inputFile, int numInstrs)
{
  int count = 0;

  char line[MAX_LINE_LENGTH];

  // Need to get past the first 3 lines in the file which identify
  // the type of pipeline, the column names, and some dashes underlining
  // column names.
  int passLines = 0;
  while(passLines < 3){
    if(readLine(inputFile, line) > 0){
      char *str = trim(line);
      if(strlen(str) > 0){
	passLines++;
      }
      if(str)
	free(str);
    }
  }

  // Allocate the array of structs
  struct line *array = (struct line*)malloc(numInstrs * sizeof(struct line));

  // Populate the array of structs from the input file stream
  while(!feof(inputFile) && count < numInstrs){
    int num = readLine(inputFile, line);
    if(num <= 0)
      continue;
    splitLine(line, &array[count]);
    count++;
  }

  // If we didn't read in enough lines, free the array and return NULL
  if(count != numInstrs){
    free(array);
    array = NULL;
  }
  
  return array;
  
}

// For the specified file name and number of instructions, create an array
// of struct line arrays, one for each pipeline type.
struct line** readFile(char *file, int numInstrs)
{
  // Allocate the array of struct line arrays
  struct line **resultArray = (struct line**)malloc(NUM_DATAPATHS *
						    sizeof(struct line*));

  // Open the file
  FILE *input = fopen(file, "r");
  if(input == NULL){
    printf("Unable to open file %s.\nExiting.\n", file);
    exit(1);
  }

  // Get the first set of input files for ideal pipeline
  struct line * noHazardLines = readDatapathLines(input, numInstrs);
  if(noHazardLines == NULL){
    printf("Nothing read for no hazard datapath from file %s\n", file);
    exit(1);
  }
  free(noHazardLines);

  /*
  // Get the first set of input files for stalling pipeline  
  struct line * stallLines = readDatapathLines(input, numInstrs);
  if(stallLines == NULL){
    printf("Nothing read for stalling datapath from file %s\n", file);
    exit(1);
  }
  */
  
  // Get the first set of input files for data forwarding pipeline  
  struct line * forwardLines = readDatapathLines(input, numInstrs);
  if(forwardLines == NULL){
    printf("Nothing read for forwarding datapath from file %s\n", file);
    exit(1);
  }  

  fclose(input);  
  
  //resultArray[STALLING] = stallLines;
  resultArray[FORWARDING] = forwardLines;

  return resultArray;
}

// Free the dynamically allocated array of struct line arrays.
void freeLines(struct line **lines)
{
  for(int i = 0; i < NUM_DATAPATHS; i++){
    free(lines[i]);
  }
  free(lines);
}

// Check to see if the two input files match for a given instruction.
// Print a message to stdout when there is a mismatch.
void checkDatapathEquality(struct line *sol, struct line *test, int numInstrs)
{
  int timeDiff = 0;
  
  for(int i = 0; i < numInstrs; i++){
    int textCmp = strcmp(sol[i].text, test[i].text);
    if(textCmp != 0){
      printf("Text mismatch \n\tsoln: %s\n \ttest: %s\n", sol[i].text,
	     test[i].text);
    }

    if(i > 0){
      int solDiff = sol[i].time - sol[i-1].time;
      int testDiff = test[i].time - test[i-1].time;
      if(solDiff != testDiff){
	printf("Time mismatch\n \ttext:%s\n \t soln offset from last instr:\t%d\n \t test offset from last instr:\t%d\n", sol[i].text, solDiff, testDiff);
	
      }
    }
  }
  
}

// For the 3 data paths, check if the times for instructions match
// across the two input files' data.
void checkEquality( struct line **sol, struct line **test, int numInstrs)
{
  for(int i = 0; i < NUM_DATAPATHS; i++){
    printf("%s\n", DatapathNames[i]);
    checkDatapathEquality(sol[i], test[i], numInstrs);
    printf("\n");
  }
}

// Print usage information for the program
void usage()
{
  printf("Usage: checker solution-file test-file num-instrs\n");
}

// Reads in two input files names and number of instructions from the
// command line.  Compares the contents of the input files to determine
// if there are any differences.  When differences occur, prints a
// message to stdout.
int main(int argc, char **argv)
{
  int numInstrs = 0;
  if(argc < 4 || (numInstrs = atoi(argv[3])) < 0){
    usage();
    exit(1);
  }

  // Read both input files and convert to internal data structures
  struct line** sourceLines = readFile(argv[1], numInstrs);
  struct line** testLines = readFile(argv[2], numInstrs);

  // Determine if the contents of the files match
  checkEquality(sourceLines, testLines, numInstrs);

  // Free dynamically allocated memory
  freeLines(sourceLines);
  freeLines(testLines);
}
