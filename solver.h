/*
//possible special case: quando uma linha das extremidades (ie antes/a seguir da qual o puzzle acaba) é resolvida parcialmente, já sabemos a disposição completa de pelo menos um dos blocos das linhas perpendiculares as celulas resolvidas. 
Por exemplo, se descobrirmos que toda a última linha é composta por #'s, entao o último bloco de todas as colunas está de imediato descoberto!
*/

#ifndef _SOLVER_INCLUDED
#define _SOLVER_INCLUDED

#define ERROR_BADFORMAT "Error! Invalid file formatting."
#define ERROR_404		"Error! File not found."
#define ERROR_MEM		"Error! Failed to allocate memory."
#define ERROR_ARGS		"Error! Invalid commandline arguments."

#define BUFFERSIZE	64

#define STATE_FULL '#'	//states are characters so that they can be printed directly
#define STATE_BLNK '-'	//
#define STATE_UNKN '?'	//

#define ROW 0	//for array indexing
#define COL 1
#define AXES 2

#define DEBUG

typedef struct _cell {
	char state;		//STATE_FULL, STATE_BLNK, STATE_UNKN
} Cell;

typedef struct _block {
	int length;
} Block;

typedef struct _line {
	Cell** cells;	//array of pointers to cells
	Block* block;	//array of blocks for this line
	int blockNum;
} Line;

typedef struct _puzzle {
	int* length;				//number of rows and columns
	Line** line;				//2 arrays of lines
} Puzzle;

void debp(const char*, ... );
void ExportSolution(Puzzle*, FILE*);
void getDimension(Puzzle*, FILE*, int);
void getBlockLengths(Puzzle*, FILE*, int);
void errorout(char* type, char* details);
void checkblankln(FILE* fp);
Puzzle* getPuzzle(char*);

Line* CloneLine(Line*, int);
Puzzle* ClonePuzzle(Puzzle*);
int getMinSumOfBlocksAndBlanks(Line*, int);
int getLengthOfLargestBlock(Line*);

int solveline(Puzzle*, Line*, Stack*, int);

void solve(Puzzle*, Stack**, int);

#endif
