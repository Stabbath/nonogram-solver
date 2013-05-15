CFLAGS = -Wall -pedantic -std=c99 -c
CC = gcc

solver: stacks.o solver.o solverio.o stocks.o presolver.o
	$(CC) -o nonograms solver.o stacks.o solverio.o stocks.o presolver.o

solver.o: solver.c solver.h stacks.h
	$(CC) $(CFLAGS) solver.c

presolver.o: presolver.c solver.h
	$(CC) $(CFLAGS) presolver.c

solverio.o: solverio.c solver.h stacks.h
	$(CC) $(CFLAGS) solverio.c

stacks.o: stacks.c stacks.h
	$(CC) $(CFLAGS) stacks.c
	
stocks.o: stocks.c solver.h
	$(CC) $(CFLAGS) stocks.c

clean:
	rm -f *.o *~
