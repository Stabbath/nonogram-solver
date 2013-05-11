#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include "stacks.h"
#include "solver.h"


/** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
  * debp: 					debug print! if DEBUG is defined, will print a string just like printf would*
  *							but flushes the output stream straight away.								*
  *																										*
  * @param char* :			string with text to be written to stdout									*
  * @param ... : 			additional args based on format												*
  *	@noreturn																							*
  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void debp(const char* format, ... ) {
	#ifdef DEBUG
		va_list args;
		va_start(args, format);
		vprintf(format, args);
		va_end(args);
		fflush(stdout);
	#endif
}



/** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
  * ExportConfig: 			Exports a puzzle's config in the same format as the input one. 			*
  *																										*
  * @param Puzzle* :		puzzle to get config from													*
  * @param FILE* : 			output stream																*
  *	@noreturn																							*
  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void ExportConfig(Puzzle* puzzle, FILE* stream) {
	int i, j;
	fprintf(stream, "%d\n", puzzle->length[ROW]);
	fprintf(stream, "%d\n", puzzle->length[COL]);
	fprintf(stream, "\n");
	for (j = 0; j < puzzle->length[ROW]; j++) {
		fprintf(stream, "%d ", puzzle->line[ROW][j].blockNum);
		for (i = 0; i < puzzle->line[ROW][j].blockNum; i++) {
			fprintf(stream, "%d ", puzzle->line[ROW][j].block[i].length);
		}
		fprintf(stream, "\n");
	}
	fprintf(stream, "\n");
	for (j = 0; j < puzzle->length[COL]; j++) {
		fprintf(stream, "%d ", puzzle->line[COL][j].blockNum);
		for (i = 0; i < puzzle->line[COL][j].blockNum; i++) {
			fprintf(stream, "%d ", puzzle->line[COL][j].block[i].length);
		}
		fprintf(stream, "\n");
	}
	fprintf(stream, "\n");
}



/** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
  * ExportSolution: O(L²)	Exports a puzzle's solution as a grid of ASCII characters (#, -, ?)			*
  *																										*
  *	NOTE: only O(1) if puzzle is NULL																	*
  *																										*
  * @param Puzzle* :		puzzle to get config from													*
  * @param FILE* : 			output stream																*
  *	@noreturn																							*
  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void ExportSolution(Puzzle* puzzle, FILE* stream) {	//O(L²)
	static int solutions = 0;
	
	
	if (puzzle == NULL) {
		if (solutions == 0) {
			//there are no solutions
		}
	} else {
		#ifdef DEBUG
//		ExportConfig(puzzle, stdout);
		#endif
		
		int i, j;
		for (j = 0; j < puzzle->length[ROW]; j++) {	//O(Lr*Lc) = O(L²)
			for (i = 0; i < puzzle->length[COL]; i++) {	//O(Lc)
				fprintf(stream, "%c", puzzle->line[COL][i].cells[j]->state);
			}
			fprintf(stream, "\n");
		}
		fprintf(stream, "\n");
		solutions++;
	}
}



/** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
  * getDimension: O(1)		Macro function to fetch a puzzle's row|column lengths.			 			*
  *																										*
  * @param Puzzle* :		puzzle we're loading														*
  * @param FILE* : 			input stream																*
  * @param int : 			ROW|COL																		*
  *	@noreturn																							*
  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void getDimension(Puzzle* puzzle, FILE* fp, int coord) {	//O(1)
	char buffer[BUFFERSIZE];
	if (fgets(buffer, sizeof(buffer), fp) == NULL)
		errorout(ERROR_BADFORMAT, "File ended unexpectedly (row num).");
	if (sscanf(buffer, "%d", &(puzzle->length[coord])) <= 0)
		errorout(ERROR_BADFORMAT, "Row number not an integer.");
}


/** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
  * readBlockLengthsRow: O(L²)	Macro function to fetch a puzzle's (row|column)'s block lengths.		*
  *																										*
  * @param Puzzle* :			puzzle we're loading													*
  * @param FILE* : 				input stream															*
  * @param int :				ROW|COL																	*
  *	@noreturn																							*
  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void readBlockLenghts(Puzzle* puzzle, FILE* fp, int coord) {	//O(L²)
	if (coord == ROW) {	//we're getting rows - O(L²)

		int i, j, n;
		char buffer[BUFFERSIZE];
		for (i = 0; i < puzzle->length[ROW]; i++) {	//O(Lr*(Lc+N)) = O(L² + L*N) = O(L²) , N << L
			if (fgets(buffer, sizeof(buffer), fp) == NULL)	errorout(ERROR_BADFORMAT, "File ended unexpectedly (block lengths).");

			sscanf(buffer, " %d", &puzzle->line[ROW][i].blockNum);
			if (puzzle->line[ROW][i].blockNum > 0) {
				if ((puzzle->line[ROW][i].block = (Block*) malloc(puzzle->line[ROW][i].blockNum*sizeof(Block))) == NULL)	errorout(ERROR_MEM, "Could not create *block[ROW].");
			} else 
			if (puzzle->line[ROW][i].blockNum == 0) {	//special case: 0 means 1 block of 0 length (ie no blocks)
				if ((puzzle->line[ROW][i].block = (Block*) malloc(sizeof(Block))) == NULL)	errorout(ERROR_MEM, "Could not create *block[ROW].");
				puzzle->line[ROW][i].block[0].length = 0;
				puzzle->line[ROW][i].blockNum = 1;
			} else {
				errorout(ERROR_BADFORMAT, "Found a negative number.");
			}
		
			if ((puzzle->line[ROW][i].cells = (Cell**) malloc(puzzle->length[COL]*sizeof(Cell*))) == NULL)	errorout(ERROR_MEM,"Could not create rows[i].cells.");
			for (j = 0; j < puzzle->length[COL]; j++) {		//O(Lc)
				puzzle->line[ROW][i].cells[j] = (Cell*) malloc(sizeof(Cell));
				puzzle->line[ROW][i].cells[j]->state = STATE_UNKN;
			}

			//note: if there are spaces before the first integer, the first number won't be 'deleted' - write function that is also mentioned a few lines of comment down
			for (n = 0; !isspace(buffer[n]); n++) {	//O(1)     "delete" first number, which determined array size
				buffer[n] = ' ';
			}

			for (j = 0; j < puzzle->line[ROW][i].blockNum; j++) {	//O(N)
	//			sscanf(buffer, " %d ", &(puzzle->line[ROW][i].block[j].length));	//get first integer
				for (; !isspace(buffer[n]); n++) {			//replace all characters of the current word (ie the fetched integer) with whitespace
					buffer[n] = ' ';						//the goal of this is to have the "first" integer be the next one, so that with a single 
				}											//sscanf attribution we can read everything
				n++;	//+1 to get starting position of next word (ie next integer) to start on next whitespace-filling loop	- this doesnt currently account for multiple spaces! need a function with a loop to skip all the spaces (or error out if more than a space is found)
				sscanf(buffer, " %d ", &(puzzle->line[ROW][i].block[j].length));	//get first integer
			}
		} 

	} else {	//we're getting columns - O(L²)

		int i, j, n;
		char buffer[BUFFERSIZE];
		for (i = 0; i < puzzle->length[COL]; i++) {	//O(Lc*(Lr+N)) = O(L² + L*N) = O(L²)
			if (fgets(buffer, sizeof(buffer), fp) == NULL)	errorout(ERROR_BADFORMAT, "File ended unexpectedly (block lengths).");

			sscanf(buffer, " %d", &puzzle->line[COL][i].blockNum);
			if (puzzle->line[COL][i].blockNum > 0) {
				if ((puzzle->line[COL][i].block = (Block*) malloc(puzzle->line[COL][i].blockNum*sizeof(Block))) == NULL)	errorout(ERROR_MEM, "Could not create *block[COL].");
			} else
			if (puzzle->line[COL][i].blockNum == 0) {	//special case: 0 means 1 block of 0 length (ie no blocks)
				if ((puzzle->line[COL][i].block = (Block*) malloc(sizeof(Block))) == NULL)	errorout(ERROR_MEM, "Could not create *block[COL].");
				puzzle->line[COL][i].block[0].length = 0;
				puzzle->line[COL][i].blockNum = 1;
			} else {
				errorout(ERROR_BADFORMAT, "Found a negative number.");
			}

			if ((puzzle->line[COL][i].cells = (Cell**) malloc(puzzle->length[COL]*sizeof(Cell*))) == NULL)	errorout(ERROR_MEM,"Could not create rows[i].cells.");
			for (j = 0; j < puzzle->length[ROW]; j++) {	//O(Lr)
				puzzle->line[COL][i].cells[j] = puzzle->line[ROW][j].cells[i];
			}

			//note: if there are spaces before the first integer, the first number won't be 'deleted' - write function that is also mentioned a few lines of comment down
			for (n = 0; !isspace(buffer[n]); n++) {	//"delete" first number, which determined array size
				buffer[n] = ' ';
			}

			for (j = 0; j < puzzle->line[COL][i].blockNum; j++) {	//O(N)
	//			sscanf(buffer, " %d ", &(puzzle->line[COL][i].block[j].length));	//get first integer
				for (; !isspace(buffer[n]); n++) {	//replace all characters of the current word (ie the fetched integer) with whitespace
					buffer[n] = ' ';						//the goal of this is to have the "first" integer be the next one, so that with a single 
				}											//sscanf attribution we can read everything
				n++;	//+1 to get starting position of next word (ie next integer) to start on next whitespace-filling loop	- this doesnt currently account for multiple spaces! need a function with a loop to skip all the spaces (or error out if more than a space is found)
				sscanf(buffer, " %d ", &(puzzle->line[COL][i].block[j].length));	//get first integer
			}
		}		
	}
}



/** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
  * errorout: 				prints an error message and exits with code 1					 			*
  *																										*
  * @param char* :			string for general macro-defined error type message							*
  * @param char* : 			string for more specific details on error									*
  *	@noreturn																							*
  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void errorout(char* type, char* details) {	//O(1)
	printf("%s %s\n", type, details);
	exit(1);
}



/** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
  * checkblankln: 			checks if there is a next line and if it contains nothing but "\n"			*
  *																										*
  * @param FILE* : 			input stream																*
  *	@noreturn																							*
  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void checkblankln(FILE* fp) {	//O(1)
	char buffer[BUFFERSIZE];
	if (fgets(buffer, sizeof(buffer), fp) == NULL)	//do we even have a line to read?
		errorout(ERROR_BADFORMAT, "File ended unexpectedly (was expecting a blank line).");
	if (strcmp(buffer, "\n") != 0)	//line is there but not blank!
		errorout(ERROR_BADFORMAT, "Blank line was not blank (check for whitespace other than \\n).");
}



/** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
  * getPuzzle: 				Macro function for reading and storing a puzzle's configuration				*
  *																										*
  * @param char* : 			name of puzzle file to get config from										*
  *	@return Puzzle* :		pointer to puzzle structure with the fetched configuration					*
  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
Puzzle* getPuzzle(char* name) {	//O(L²)
	FILE* fp = fopen(name, "r");
	if (fp == NULL)	errorout(ERROR_404, "Wrong directory or name for puzzle config.");
	Puzzle* puzzle = (Puzzle*) malloc(sizeof(Puzzle));
	puzzle->name = name;
	
	if (puzzle == NULL) errorout(ERROR_MEM, "Could not create Puzzle.");
	
	if ((puzzle->length = (int*) malloc(AXES*sizeof(int))) == NULL)	errorout(ERROR_MEM, "Could not create lengths.");
	
	//O(1) routines
	getDimension(puzzle, fp, ROW);	//get 1st line - L = number of rows
	getDimension(puzzle, fp, COL);	//get 2nd line - C = number of columns
	checkblankln(fp);				//3rd line - must be blank

	if ((puzzle->line = (Line**) malloc(AXES*sizeof(Line*))) == NULL)	errorout(ERROR_MEM, "Could not create lines.");	
	if ((puzzle->line[ROW] = (Line*) malloc(puzzle->length[ROW]*sizeof(Line))) == NULL)	errorout(ERROR_MEM, "Could not create rows.");
	if ((puzzle->line[COL] = (Line*) malloc(puzzle->length[COL]*sizeof(Line))) == NULL)	errorout(ERROR_MEM, "Could not create cols.");

	//O(L²) routines
	readBlockLenghts(puzzle, fp, ROW);	//next L lines have row block lengths
	checkblankln(fp);					//get (3+L+1)th line - must be blank
	readBlockLenghts(puzzle, fp, COL);	//next C lines have col block lengths
	checkblankln(fp);					//get (3+L+1+C+1)th line - must be blank

	char buffer[BUFFERSIZE];
	if (fgets(buffer, sizeof(buffer), fp) != NULL) {	//file must end after previous blank line
		errorout(ERROR_BADFORMAT, "File did not end when expected.");
	}

	/*get basic mins and maxes for each block*/
	int i, j, n, min, max, length;
	Line* line;
	for (j = ROW; j < AXES; j++) {
		for (i = 0; i < puzzle->length[j]; i++) {
			line = &puzzle->line[j][i];
			length = puzzle->length[opAxis(j)];
			min = 0;
			max = length - 1 - (getMinSumOfBlocksAndBlanks(line, 0) - line->block[0].length);
			for (n = 0; n < line->blockNum; n++) {
				line->block[n].min = min;
				line->block[n].max = max;
				
				if (n < line->blockNum - 1) {
					min += line->block[n].length + 1;
					max += line->block[n + 1].length + 1;
				}
			}
		}
	}
	
	fclose(fp);
	
	return puzzle;
}
