# Advanced Makefile commands

In this README, you will put the output to several commands you are
asked to run in the Makefile portion of part B of lab 4.

This README is written using [Markdown](https://www.markdownguide.org/).
Please format your answers using Markdown so that it displays neatly
in your GitHub repository's web interface.
You can check your formatting by pushing your code and navigating to your
README within your private GitHub repository.

## Questions
Complete the questions below in the space provided.

 * Indicate in the tick marks below the output of the command `make hello`
   ``` 
   gcc -g -Wall hello.c -o hello
   ```

 * Indicate in the tick marks below the output of the command `make` after doing a `make clean` and `rm .depend`
   ```
   rm -f ./.depend
   gcc -g -Wall -MM util.c parser.c isa.c instruction.c registers.c sim.c -MF ./.depend;
   gcc -g -Wall -c util.c
   gcc -g -Wall -c parser.c
   gcc -g -Wall -c isa.c
   gcc -g -Wall -c instruction.c
   gcc -g -Wall -c registers.c
   gcc -g -Wall -c sim.c	
   ```

 * Indicate in the tick marks below the contents of the file `.depend` after
running the command `make`
   ```
   sim.o: sim.c isa.h registers.h parser.h instruction.h util.h sim.h
   ```


