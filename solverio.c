#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "stacks.h"
#include "solver.h"

/** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
  * ExportSolution: O(L²)	Exports a puzzle's solution as a grid of ASCII characters (#, -, ?) to a	*
  *							properly named file.														*
  *																										*
  *	NOTE: O(1) if puzzle is NULL																		*
  *																										*
  * @param Puzzle* :		puzzle to get config from													*
  * @param char* :			explicit name string for when puzzle == NULL. Unused if puzzle != NULL		*
  *	@noreturn																							*
  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void ExportSolution(Puzzle* puzzle, char* name) {	//O(L²)
	static int solutions = 0;

	if (puzzle == NULL) {	//called with NULL at the end of the program every time
		if (solutions == 0) {	//means we reached the end without finding any solutions
			char filename[BUFFERSIZE];
			strcpy(filename, name);
			strcat(filename, ".sol");
			FILE* stream = fopen(filename, "w");
			fclose(stream);
		}
	} else {
		char filename[BUFFERSIZE];
		strcpy(filename, puzzle->name);
	
		/* while identifying multiple solutions is not required, it's still a good thing to do */
		if (solutions > 0) {	//for multiple solutions use a different filename format
			strcat(filename, "-");

			char num[BUFFERSIZE];
			sprintf(num, "%d", solutions);
			strcat(filename, num);
		}
		strcat(filename, ".sol");

		/* create file and write out the solution */
		FILE* stream = fopen(filename, "w");
		int i, j;
		for (j = 0; j < puzzle->length[ROW]; j++) {
			for (i = 0; i < puzzle->length[COL]; i++) {
				fprintf(stream, "%c", puzzle->line[COL][i].cells[j]->state);
			}
			fprintf(stream, "\n");	//line break at end of each row
		}
		fprintf(stream, "\n");	//line break at end of puzzle for extra blank line
		solutions++;
		fclose(stream);
	}
}



/** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
  * getDimension: O(1)		Macro function to fetch a puzzle's row|column lengths.			 			*
  *																										*
  * @param Puzzle* :		puzzle we're setting up														*
  * @param FILE* : 			input stream																*
  * @param int : 			ROW|COL																		*
  *	@noreturn																							*
  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void getDimension(Puzzle* puzzle, FILE* fp, int coord) {	//O(1)
	char buffer[BUFFERSIZE];
	fgets(buffer, sizeof(buffer), fp);
	sscanf(buffer, "%d", &(puzzle->length[coord]));
}


/** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
  * readBlockLengths: O(L²)		Macro function to fetch a puzzle's (row|column)'s block lengths.		*
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
			fgets(buffer, sizeof(buffer), fp);

			sscanf(buffer, " %d", &puzzle->line[ROW][i].blockNum);
			if (puzzle->line[ROW][i].blockNum > 0) {
				if ((puzzle->line[ROW][i].block = (Block*) malloc(puzzle->line[ROW][i].blockNum*sizeof(Block))) == NULL)	errorout(ERROR_MEM, "Could not create *block[ROW].");
			} else 
			if (puzzle->line[ROW][i].blockNum == 0) {	//special case: 0 means 1 block of 0 length (ie no blocks)
				if ((puzzle->line[ROW][i].block = (Block*) malloc(sizeof(Block))) == NULL)	errorout(ERROR_MEM, "Could not create *block[ROW].");
				puzzle->line[ROW][i].block[0].length = 0;
				puzzle->line[ROW][i].blockNum = 1;
			}
			
			puzzle->line[ROW][i].unsolvedCells = puzzle->length[COL];

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
			fgets(buffer, sizeof(buffer), fp);

			sscanf(buffer, " %d", &puzzle->line[COL][i].blockNum);
			if (puzzle->line[COL][i].blockNum > 0) {
				if ((puzzle->line[COL][i].block = (Block*) malloc(puzzle->line[COL][i].blockNum*sizeof(Block))) == NULL)	errorout(ERROR_MEM, "Could not create *block[COL].");
			} else
			if (puzzle->line[COL][i].blockNum == 0) {	//special case: 0 means 1 block of 0 length (ie no blocks)
				if ((puzzle->line[COL][i].block = (Block*) malloc(sizeof(Block))) == NULL)	errorout(ERROR_MEM, "Could not create *block[COL].");
				puzzle->line[COL][i].block[0].length = 0;
				puzzle->line[COL][i].blockNum = 1;
			}
			
			puzzle->line[COL][i].unsolvedCells = puzzle->length[ROW];

			if ((puzzle->line[COL][i].cells = (Cell**) malloc(puzzle->length[ROW]*sizeof(Cell*))) == NULL)	errorout(ERROR_MEM,"Could not create rows[i].cells.");
			for (j = 0; j < puzzle->length[ROW]; j++) {	//O(Lr)
				puzzle->line[COL][i].cells[j] = puzzle->line[ROW][j].cells[i];
			}

			//note: if there are spaces before the first integer, the first number won't be 'deleted' - write function that is also mentioned a few lines of comment down
			for (n = 0; !isspace(buffer[n]); n++) {	//"delete" first number, which determined array size
				buffer[n] = ' ';
			}

			for (j = 0; j < puzzle->line[COL][i].blockNum; j++) {	//O(N)
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
  * checkblankln: 			since input will always be correctly formatted, simply advances the			*
  *							file pointer to the next line												*
  *																										*
  * @param FILE* : 			input stream																*
  *	@noreturn																							*
  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void checkblankln(FILE* fp) {	//O(1)
	char buffer[BUFFERSIZE];
	fgets(buffer, sizeof(buffer), fp);
}



/** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
  * getPuzzle: 				Macro function for reading and storing a puzzle's configuration				*
  *																										*
  * @param char* : 			name of puzzle file to get config from										*
  *	@return Puzzle* :		pointer to puzzle structure with the fetched configuration					*
  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
Puzzle* getPuzzle(char* name) {	//O(L²)
	 /* * * * * * * * *
	  * name handling *
	  * * * * * * * * */
	int c = 0;
	int dot = -1;
	while (name[c] != '\0') {
		if (name[c] == '.') {
			dot = c;
		}
		c++;
	}
	char ext[BUFFERSIZE];
	strcpy(ext, &name[dot]);
	//files named simply '.cfg' will be openable
	if (dot == -1 || strcmp(ext, ".cfg")) errorout(ERROR_ARGS, "Can only open *.cfg files!");

	FILE* fp = fopen(name, "r");
	if (fp == NULL)	errorout(ERROR_404, "Wrong directory or name for puzzle config.");
	


	 /* * * * * * * * * * * * * * * * * * * * * *
	  * file processing and memory allocations  *
	  * * * * * * * * * * * * * * * * * * * * * */
	Puzzle* puzzle = (Puzzle*) malloc(sizeof(Puzzle));
	if (puzzle == NULL) errorout(ERROR_MEM, "Could not create Puzzle.");

	name[dot] = '\0'; 		//cut out the extension
	puzzle->name = name;	//and pass it along to puzzle
	
	if ((puzzle->length = (int*) malloc(AXES*sizeof(int))) == NULL)	 errorout(ERROR_MEM, "Could not create length array.");
	
	//O(1) basic info routines
	getDimension(puzzle, fp, ROW);	//get 1st line - L = number of rows
	getDimension(puzzle, fp, COL);	//get 2nd line - C = number of columns
	checkblankln(fp);				//3rd line - must be blank

	if ((puzzle->line = (Line**) malloc(AXES*sizeof(Line*))) == NULL)	errorout(ERROR_MEM, "Could not create lines.");	
	if ((puzzle->line[ROW] = (Line*) malloc(puzzle->length[ROW]*sizeof(Line))) == NULL)	errorout(ERROR_MEM, "Could not create rows.");
	if ((puzzle->line[COL] = (Line*) malloc(puzzle->length[COL]*sizeof(Line))) == NULL)	errorout(ERROR_MEM, "Could not create cols.");

	//O(L²) line-preparing routines
	readBlockLenghts(puzzle, fp, ROW);	//next L lines have row block lengths
	checkblankln(fp);					//get (3+L+1)th line - must be blank
	readBlockLenghts(puzzle, fp, COL);	//next C lines have col block lengths
	checkblankln(fp);					//get (3+L+1+C+1)th line - must be blank

	fclose(fp);
	
	return puzzle;
}
