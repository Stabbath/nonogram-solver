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
	char state;			//STATE_FULL, STATE_BLNK, STATE_UNKN
	struct _line* row;	//row this cell belongs to
	struct _line* col;	//column this cell belongs to
} Cell;

typedef struct _block {
	int length;	//how long is this block
	int min;	//what's the lowest cell index that can possibly be a part of this block
	int max;	//what's the highest cell index that can possibly be a part of this block
} Block;

typedef struct _line {
	Cell** cells;	//array of pointers to cells
	Block* block;	//array of blocks for this line
	int blockNum;	//number of blocks in this line
	int unsolvedCells;	//number of unknowns ('?') in this line
} Line;

typedef struct _puzzle {
	char* name;		//name of input puzzle, minus extension
	int* length;	//number of rows and columns
	Line** line;	//2 arrays of lines (1 for rows, 1 for columns)
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
int 	getSumOfBlocks					(Line*);
int 	checkline						(Line*, 	int);
int 	checkpuzzle						(Puzzle*);
void 	LinkCellsToLines				(Puzzle*);
void 	SetupMinsAndMaxes				(Puzzle*);

//presolver.c
int 	presolve	(Puzzle*);
int 	stackline	(Line*, 	int);

//solver.c - also contains main
Line* 	MergeBlockPositions	(Line*, 	int, 		int);
void 	ExamineBlocks		(Line*, 	int, 		int, 		int , 	Stack*, 	int);
int 	solveline			(Puzzle*, 	Stack**, 	Stack*, 	int);
void 	solve				(Puzzle*, 	Stack**, 	Stack*, 	int);


#endif
