#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include "stacks.h"
#include "solver.h"

#define DEBUG

void debp(const char* format, ... ) {
	#ifdef DEBUG
		va_list args;
    	va_start(args, format);
    	vprintf(format, args);
    	va_end(args);
    	fflush(stdout);
	#endif
}

void ExportSolution(Puzzle* puzzle) {
	static int solutions = 0;

	if (puzzle == NULL && solutions == 0) {
		//there are no solutions
	} else {
		int i, j;
		for (j = 0; j < puzzle->length[COL]; j++) {
			for (i = 0; i < puzzle->length[ROW]; i++) {
				printf("%c", puzzle->line[ROW][i].cells[j]->state);
			}
			printf("\n");
		}

/*		printf("\n");	//to make sure rows and columns point to the same cells
		for (j = 0; j < puzzle->length[COL]; j++) {
			for (i = 0; i < puzzle->length[ROW]; i++) {
				printf("%d ", (int) puzzle->line[ROW][i].cells[j]);
			}
			printf("\n");
		}

		printf("\n");
		for (j = 0; j < puzzle->length[COL]; j++) {
			for (i = 0; i < puzzle->length[ROW]; i++) {
				printf("%d ", (int) puzzle->line[COL][j].cells[i]);
			}
			printf("\n");
		}*/
	}
}

void getDimension(Puzzle* puzzle, FILE* fp, int coord) {
	char buffer[BUFFERSIZE];
	if (fgets(buffer, sizeof(buffer), fp) == NULL)
		errorout(ERROR_BADFORMAT, "File ended unexpectedly (row num).");
	if (sscanf(buffer, "%d", &(puzzle->length[coord])) <= 0)
		errorout(ERROR_BADFORMAT, "Row number not an integer.");
}

void readBlockLenghtsRow(Puzzle* puzzle, FILE* fp) {
	int i, j, n;
	char buffer[BUFFERSIZE];
	for (i = 0; i < puzzle->length[ROW]; i++) {
		if (fgets(buffer, sizeof(buffer), fp) == NULL)	errorout(ERROR_BADFORMAT, "File ended unexpectedly (block lengths).");

		sscanf(buffer, " %d", &puzzle->line[ROW][i].blockNum);
		if (puzzle->line[ROW][i].blockNum > 0) {
			if ((puzzle->line[ROW][i].block = (Block*) malloc(puzzle->line[ROW][i].blockNum*sizeof(Block))) == NULL)	errorout(ERROR_MEM, "Could not create *block[ROW].");
		} else 
		if (puzzle->line[ROW][i].blockNum == 0) {	//special case: 0 means 1 block of 0 length (ie no blocks)
			if ((puzzle->line[ROW][i].block = (Block*) malloc(sizeof(Block))) == NULL)	errorout(ERROR_MEM, "Could not create *block[ROW].");
			puzzle->line[ROW][i].block[0].length = 0;
		} else {
			errorout(ERROR_BADFORMAT, "Found a negative number.");
		}
		
		if ((puzzle->line[ROW][i].cells = (Cell**) malloc(puzzle->length[COL]*sizeof(Cell*))) == NULL)	errorout(ERROR_MEM,"Could not create rows[i].cells.");
		for (j = 0; j < puzzle->length[COL]; j++) {
			puzzle->line[ROW][i].cells[j] = (Cell*) malloc(sizeof(Cell));
			puzzle->line[ROW][i].cells[j]->state = STATE_UNKN;
		}

		//note: if there are spaces before the first integer, the first number won't be 'deleted' - write function that is also mentioned a few lines of comment down
		for (n = 0; !isspace(buffer[n]); n++) {	//"delete" first number, which determined array size
			buffer[n] = ' ';
		}

		for (j = 0; j < puzzle->line[ROW][i].blockNum; j++) {	//read arraySize blocks
			sscanf(buffer, " %d ", &(puzzle->line[ROW][i].block[j].length));	//get first integer
			for (; !isspace(buffer[n]); n++) {	//replace all characters of the current word (ie the fetched integer) with whitespace
				buffer[n] = ' ';						//the goal of this is to have the "first" integer be the next one, so that with a single 
			}											//sscanf attribution we can read everything
			n++;	//+1 to get starting position of next word (ie next integer) to start on next whitespace-filling loop	- this doesnt currently account for multiple spaces! need a function with a loop to skip all the spaces (or error out if more than a space is found)
		}
	}
}

void readBlockLenghtsCol(Puzzle* puzzle, FILE* fp) {
	int i, j, n;
	char buffer[BUFFERSIZE];
	for (i = 0; i < puzzle->length[COL]; i++) {
		if (fgets(buffer, sizeof(buffer), fp) == NULL)	errorout(ERROR_BADFORMAT, "File ended unexpectedly (block lengths).");

		sscanf(buffer, " %d", &puzzle->line[COL][i].blockNum);
		if (puzzle->line[COL][i].blockNum > 0) {
			if ((puzzle->line[COL][i].block = (Block*) malloc(puzzle->line[COL][i].blockNum*sizeof(Block))) == NULL)	errorout(ERROR_MEM, "Could not create *block[COL].");
		} else
		if (puzzle->line[COL][i].blockNum == 0) {	//special case: 0 means 1 block of 0 length (ie no blocks)
			if ((puzzle->line[COL][i].block = (Block*) malloc(sizeof(Block))) == NULL)	errorout(ERROR_MEM, "Could not create *block[COL].");
			puzzle->line[COL][i].block[0].length = 0;
		} else {
			errorout(ERROR_BADFORMAT, "Found a negative number.");
		}

		if ((puzzle->line[COL][i].cells = (Cell**) malloc(puzzle->length[COL]*sizeof(Cell*))) == NULL)	errorout(ERROR_MEM,"Could not create rows[i].cells.");
		for (j = 0; j < puzzle->length[COL]; j++) {
			puzzle->line[COL][i].cells[j] = puzzle->line[ROW][j].cells[i];
		}

		//note: if there are spaces before the first integer, the first number won't be 'deleted' - write function that is also mentioned a few lines of comment down
		for (n = 0; !isspace(buffer[n]); n++) {	//"delete" first number, which determined array size
			buffer[n] = ' ';
		}

		for (j = 0; j < puzzle->line[COL][i].blockNum; j++) {	//read arraySize blocks
			sscanf(buffer, " %d ", &(puzzle->line[COL][i].block[j].length));	//get first integer
			for (; !isspace(buffer[n]); n++) {	//replace all characters of the current word (ie the fetched integer) with whitespace
				buffer[n] = ' ';						//the goal of this is to have the "first" integer be the next one, so that with a single 
			}											//sscanf attribution we can read everything
			n++;	//+1 to get starting position of next word (ie next integer) to start on next whitespace-filling loop	- this doesnt currently account for multiple spaces! need a function with a loop to skip all the spaces (or error out if more than a space is found)
		}
	}
}

void errorout(char* type, char* details) {
	printf("%s %s\n", type, details);
	exit(1);
}

void checkblankln(FILE* fp) {
	char buffer[BUFFERSIZE];
	if (fgets(buffer, sizeof(buffer), fp) == NULL)	//do we even have a line to read?
		errorout(ERROR_BADFORMAT, "File ended unexpectedly (was expecting a blank line).");
	if (strcmp(buffer, "\n") != 0)	//line is there but not blank!
		errorout(ERROR_BADFORMAT, "Blank line was not blank.");
}

Puzzle* getPuzzle(char* name) {
	FILE* fp = fopen(name, "r");
	if (fp == NULL)	errorout(ERROR_404, "Wrong directory or name for puzzle config.");
	Puzzle* puzzle = (Puzzle*) malloc(sizeof(Puzzle));
	if (puzzle == NULL) errorout(ERROR_MEM, "Could not create Puzzle.");
	
	if ((puzzle->length = (int*) malloc(AXES*sizeof(int))) == NULL)	errorout(ERROR_MEM, "Could not create lengths.");
	
	getDimension(puzzle, fp, ROW);	//get 1st line - L = number of rows
	getDimension(puzzle, fp, COL);	//get 2nd line - C = number of columns
	checkblankln(fp);				//3rd line - must be blank

	if ((puzzle->line = (Line**) malloc(AXES*sizeof(Line*))) == NULL)	errorout(ERROR_MEM, "Could not create lines.");	
	if ((puzzle->line[ROW] = (Line*) malloc(puzzle->length[COL]*sizeof(Line))) == NULL)	errorout(ERROR_MEM, "Could not create rows.");
	if ((puzzle->line[COL] = (Line*) malloc(puzzle->length[ROW]*sizeof(Line))) == NULL)	errorout(ERROR_MEM, "Could not create cols.");

	readBlockLenghtsRow(puzzle, fp);	//next L lines have row block lengths
	checkblankln(fp);				//get (3+L+1)th line - must be blank
	readBlockLenghtsCol(puzzle, fp);	//next C lines have col block lengths
	checkblankln(fp);				//get (3+L+1+C+1)th line - must be blank

	char buffer[BUFFERSIZE];
	if (fgets(buffer, sizeof(buffer), fp) != NULL) {	//file must end after previous blank line
		errorout(ERROR_BADFORMAT, "File did not end when expected.");
	}

	return puzzle;
}
