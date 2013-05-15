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
#define MAXPATH		BUFFERSIZE

#define STATE_FULL '#'	//states are characters so that they can be printed directly
#define STATE_BLNK '-'	//and use up less memory
#define STATE_UNKN '?'	//no point encoding them as integers!

#define ROW 0	//for array indexing
#define COL 1
#define AXES 2

typedef struct _cell {
	char state;		//STATE_FULL, STATE_BLNK, STATE_UNKN
	struct _line* row;
	struct _line* col;
} Cell;

typedef struct _block {
	int length;
	int min;
	int max;
} Block;

typedef struct _line {
	Cell** cells;	//array of pointers to cells
	Block* block;	//array of blocks for this line
	int blockNum;
	int unsolvedCells;
} Line;

typedef struct _puzzle {
	char* name;
	int* length;				//number of rows and columns
	Line** line;				//2 arrays of lines
} Puzzle;

//solverio.c
void 	ExportSolution		(Puzzle*,	char*);
void 	getDimension		(Puzzle*, 	FILE*,	int);
void 	readBlockLenghts	(Puzzle*, 	FILE*,	int);
void 	errorout			(char*	, 	char*);
void 	checkblankln		(FILE*);
Puzzle*	getPuzzle			(char*);

//stocks.c
int		getMinSumOfBlocksAndBlanks		(Line*,		int);
int		getMinSumOfBlocksAndBlanksPrev	(Line*,		int);
int		getLengthOfLargestBlock			(Line*);
void 	FreePuzzle						(Puzzle*);
void 	FreeStacks						(Stack**);
Stack** InitStacks						(Puzzle*);
Cell* 	PickCell						(Puzzle*,	int*,	int*);
void 	ConditionalPush					(Stack*,	Line*);


int checkpuzzle(Puzzle* puzzle);


//presolver.c
int presolve(Puzzle* puzzle);
int stackline(Line* line, int length);


#endif
