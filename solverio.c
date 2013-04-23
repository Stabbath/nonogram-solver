#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "stacks.h"
#include "solver.h"

void ExportSolution(Puzzle* puzzle) {
	static int solutions = 0;
	
	if (puzzle == NULL && solutions == 0) {
		//there are no solutions
	} else {
		int i, j;
		for (j = 0; j < puzzle->length[COL]; j++) {
			for (i = 0; i < puzzle->length[ROW]; i++) {
				printf("%c", puzzle->line[ROW]->cells[i]->state);
			}
			printf("\n");
		}
	}
}

void copyGridToLines(Puzzle* puzzle, Cell*** cells) {
	int i, j;
	for (i = 0; i < puzzle->length[ROW]; i++) {
		if ((puzzle->line[ROW][i].cells = (Cell**) malloc(puzzle->length[ROW]*sizeof(Cell*))) == NULL)	errorout(ERROR_MEM,"Could not create rows[i].cells.");
		for (j = 0; j < puzzle->length[COL]; j++) {
			puzzle->line[ROW][i].cells[j] = cells[i][j];
		}
	}
	for (i = 0; i < puzzle->length[COL]; i++) {
		if ((puzzle->line[COL][i].cells = (Cell**) malloc(puzzle->length[COL]*sizeof(Cell*))) == NULL)	errorout(ERROR_MEM, "Could not create cols[i].cells.");
		for (j = 0; j < puzzle->length[ROW]; j++) {
			puzzle->line[COL][i].cells[j] = cells[j][i];
		}
	}
}

Cell*** createCellGrid(Puzzle* puzzle) {
	int i, j;
	Cell*** cells;
	if ((cells = (Cell***) malloc(puzzle->length[COL]*sizeof(Cell**))) == NULL)	errorout(ERROR_MEM, "Could not create ***cells.");
	for (i = 0; i < puzzle->length[COL]; i++) {
		if ((cells[i] = (Cell**) malloc(puzzle->length[ROW]*sizeof(Cell*))) == NULL)	errorout(ERROR_MEM, "Could not create **cells.");
		for (j = 0; j < puzzle->length[ROW]; j++) {
			if ((cells[i][j] = (Cell*) malloc(sizeof(Cell))) == NULL)	errorout(ERROR_MEM, "Could not create *cells.");
			cells[i][j]->state = STATE_UNKN;
		}
	}
	return cells;
}

void getDimension(Puzzle* puzzle, FILE* fp, int coord) {
	char buffer[BUFFERSIZE];
	if (fgets(buffer, sizeof(buffer), fp) == NULL)
		errorout(ERROR_BADFORMAT, "File ended unexpectedly (row num).");
	if (sscanf(buffer, "%d", &(puzzle->length[coord])) <= 0)
		errorout(ERROR_BADFORMAT, "Row number not an integer.");
}

void getBlockLengths(Puzzle* puzzle, FILE* fp, int x) {
	int i, j, n, index; 
	char buffer[BUFFERSIZE];
	for (i = 0; i < puzzle->length[x]; i++) {	//next L|C lines have row|column block lengths
		if (fgets(buffer, sizeof(buffer), fp) == NULL)	errorout(ERROR_BADFORMAT, "File ended unexpectedly (block lengths).");

		sscanf(buffer, " %d", &puzzle->line[x][i].blockNum);		//careful with the case of having 0 blocks! the read line will be simply '0'
		if (puzzle->line[x][i].blockNum > 0) {
			if ((puzzle->line[x][i].block = (Block*) malloc(puzzle->line[x][i].blockNum*sizeof(Block))) == NULL)	errorout(ERROR_MEM, "Could not create *block[ROW].");
		} else {	//special case: 0 means 1 block of 0 length (ie no blocks) (negative numbers will be considered 0)
			if ((puzzle->line[x][i].block = (Block*) malloc(sizeof(Block))) == NULL)	errorout(ERROR_MEM, "Could not create *block[ROW].");
			puzzle->line[x][i].block[0].length = 0;
			continue;
		}

		for (n = 0, index = 0; !isspace(buffer[n]); n++) {	//"delete" first number, which determined array size
			buffer[n] = ' ';	//why am i doing this weird stuff with n and index? can't remember
			index = n + 1;
		}

		for (j = 0; j < puzzle->line[x][i].blockNum; j++) {	//read arraySize blocks
			sscanf(buffer, " %d ", &(puzzle->line[x][i].block[j].length));	//get first integer
			for (n = index; !isspace(buffer[n]); n++) {	//replace all characters of the current word (ie the fetched integer) with whitespace
				buffer[n] = ' ';						//the goal of this is to have the "first" integer be the next one, so that with a single 
			}											//sscanf attribution we can read everything
			index = n + 1;	//store starting position of next word (ie next integer) to start on next whitespace-filling loop
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
	if ((puzzle->line = (Line**) malloc(AXES*sizeof(Line*))) == NULL)	errorout(ERROR_MEM, "Could not create lines.");	
	
	getDimension(puzzle, fp, ROW);	//get 1st line - L = number of rows
	getDimension(puzzle, fp, COL);	//get 2nd line - C = number of columns
	checkblankln(fp);				//3rd line - must be blank
	
	if ((puzzle->line[ROW] = (Line*) malloc(puzzle->length[ROW]*sizeof(Line))) == NULL)	errorout(ERROR_MEM, "Could not create rows.");
	if ((puzzle->line[COL] = (Line*) malloc(puzzle->length[COL]*sizeof(Line))) == NULL)	errorout(ERROR_MEM, "Could not create cols.");

	getBlockLengths(puzzle, fp, ROW);	//next L lines have row block lengths
	checkblankln(fp);				//get (3+L+1)th line - must be blank
	getBlockLengths(puzzle, fp, COL);	//next C lines have col block lengths
	checkblankln(fp);				//get (3+L+1+C+1)th line - must be blank

	char buffer[BUFFERSIZE];
	if (fgets(buffer, sizeof(buffer), fp) != NULL) {	//file must end after previous blank line
		errorout(ERROR_BADFORMAT, "File did not end when expected.");
	}

	Cell*** cells = createCellGrid(puzzle);;	//buffer so rows and columns that have a cell in common *POINT* to the same cell
	copyGridToLines(puzzle, cells);
	puzzle->cellptr = cells;

	return puzzle;
}
