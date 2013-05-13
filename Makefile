solver: stacks.o queues.o solver.o solverio.o stocks.o
	gcc -g -Wall -o solver solver.o stacks.o solverio.o stocks.o

solver.o: solver.c solver.h stacks.h
	gcc -g -Wall -c solver.c

solverio.o: solverio.c solver.h stacks.h
	gcc -g -Wall -c solverio.c

stacks.o: stacks.c stacks.h
	gcc -g -Wall -c stacks.c

queues.o: queues.c queues.h
	gcc -g -Wall -c queues.c

stocks.o: stocks.c solver.h
	gcc -g -Wall -c stocks.c

clean:
	rm -f *.o *~
