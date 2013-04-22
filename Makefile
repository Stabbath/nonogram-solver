OPTS = -g -Wall

nonogram-solver: project.c stacks.o
	gcc $(OPTS) -o project project.c stacks.o

stacks.o: stacks.c stacks.h
	gcc $(OPTS) stacks.c

clean:
	rm -f *.o *~



