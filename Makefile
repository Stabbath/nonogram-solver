solver: stacks.o solver.o solverio.o
	gcc -g -Wall -o solver solver.o stacks.o solverio.o

solver.o: solver.c solver.h stacks.h
	gcc -g -Wall -c solver.c

solverio.o: solverio.c solver.h stacks.h
	gcc -g -Wall -c solverio.c

stacks.o: stacks.c stacks.h
	gcc -g -Wall -c stacks.c

clean:
	rm -f *.o *~
