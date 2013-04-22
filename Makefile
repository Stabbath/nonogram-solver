OPTS = -g -c -Wall -ansi -pedantic

nonogram-solver: stacks.o
	gcc project.c -o project stacks.o

stacks.o: stacks.c
	gcc $(OPTS) stacks.c

clean:
	rm -f *.o *~



