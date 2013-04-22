/*
//maybe track number of unsolved cells?
//guardar posicoes de começo e fim de cada bloco (não só os X blocos da linha, mas todos os blocos identificáveis ao longo da procura da solução. por exemplo um bloco de 3 de que é desconhecido o quadrado do meio pode ser considerado 2 blocos)

*1 //talvez guardar a posicao minima e maxima de começo para cada um dos blocos para determinar quais os blocos que podemos encontrar em cada parte
*2 //se houver um vazio antes do a, seria preciso continuar a preencher para a direita do b <- caso especial?

melhora_solucoes:
	le_quadrado
	se (quadrado é conhecido e nem igual ao anterior nem dentro do mesmo bloco que o anterior)
		regista posicao
	if (distancia ao anterior < comprimento de um dos bloco que estamos à espera de encontrar nesta zona)  *1
		preenche-intermédios

//quando se descobre um bloco por completo, é preciso afixar vazios nas extremidades 
//nao esquecer que os limites do puzzle tambem sao obstaculos, semelhantes aos vazios	
//possible special case: quando uma linha das extremidades (ie antes/a seguir da qual o puzzle acaba) é resolvida parcialmente, já sabemos a posição de pelo menos um dos blocos das linhas perpendiculares as celulas resolvidas
	
preenche-intermédios:
  a    b
xx#xxxx-	//volta para trás do a se o comprimento do bloco mais pequeno que esperamos encontrar for maior a distancia entre a e b
  #xxxx#	//preenche tudo o que está no meio *2
  -    #	//é um novo começo de bloco, não se preenche nada
  -oooo-	//se o comprimento do bloco mais pequeno que esperamos encontrar for maior que o espaço entre a e b, preencher com vazios
		
	//nota: este algoritmo assume que os quadrados descobertos são guardados numa lista para depois a linha/coluna perpendicular ser re-resolvida
1. resolve todas as linhas
2. resolve todas as colunas
3. se número de desconhecidos for inferior ao resultado da última iteração -> volta para 1
4. se desconhecidos = 0, salta para 6
5. forca_solucoes
6. done

//se a solução for válida, será preciso voltar atrás à mesma, já que apenas descobrimos uma solução: podem haver mais

provavelmente vão ser preciso mais estruturas dentro da estrutura puzzle, para guardar informação em relação a cada bloco - onde começa, onde acaba, se está completamente descoberto, possivelmente mais coisas
*/

//remember this:
//guardar o numero da coluna/linha perpendicular à linha/coluna em que cada ponto é descoberto numa lista FIFO, não repetir números

//cut down on {'s on for's and if's and whatnot, code needs shortening and readabilitifying

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "stacks.h"

#define ERROR_BADFORMAT "Error! Invalid file formatting."
#define ERROR_404		"Error! File not found."
#define ERROR_MEM		"Error! Failed to allocate memory."

#define BUFFERSIZE	64

#define STATE_FULL '#'	//states are characters so that they can be printed directly
#define STATE_BLNK '-'	//
#define STATE_UNKN '?'	//

#define ROW 0	//for array indexing
#define COL 1

typedef struct _cell {
	char state;		//STATE_FULL, STATE_BLNK, STATE_UNKN
} Cell;

typedef struct _line {
	Cell** cells;	//array of pointers to cells
	int* blks;		//array of block legths for this line
} Line;

typedef struct _puzzle {
	int* length;				//number of rows and columns
	int*** blks;				//2 2d arrays of integers for block lengths
	Line** line;				//2 arrays of line pointers
} Puzzle;

void errorout(char* type, char* details) {
	printf("%s %s\n", type, details);
	exit(1);
}

void checkblankln(FILE* fp) {
	char buffer[BUFFERSIZE];
	if (fgets(buffer, sizeof(buffer), fp) == NULL)	//do we even have a line to read?
		errorout(ERROR_BADFORMAT, "File ended unexpectedly (blank line).");
	if (strlen(buffer) != 0)	//line is there but not blank!
		errorout(ERROR_BADFORMAT, "Blank line was not blank.");
}

Puzzle* getPuzzle(char* name) {
	FILE* fp = fopen(name, "r");

	if (fp == NULL)	errorout(ERROR_404, "Wrong directory or name for puzzle config.");
	
	Puzzle* puzzle = (Puzzle*) malloc(sizeof(Puzzle));
	if (puzzle == NULL) errorout(ERROR_MEM, "Could not create Puzzle.");

	Cell*** cells;	//buffer so lines that have the same cell *POINT* to the same cell
	
	char buffer[BUFFERSIZE];
	if (fgets(buffer, sizeof(buffer), fp) == NULL)	//get 1st line - L = number of rows
		errorout(ERROR_BADFORMAT, "File ended unexpectedly (row num).");
	if (sscanf(buffer, "%d", &(puzzle->length[ROW])) <= 0)
		errorout(ERROR_BADFORMAT, "Row number not an integer.");
	if (fgets(buffer, sizeof(buffer), fp) == NULL)	//get 2nd line - C = number of columns
		errorout(ERROR_BADFORMAT, "File ended unexpectedly (column num).");
	if (sscanf(buffer, "%d", &(puzzle->length[COL])) <= 0)
		errorout(ERROR_BADFORMAT, "Column number not an integer.");	
	checkblankln(fp);	//3rd line - must be blank
	
	if ((puzzle->blks = (int***) malloc(2*sizeof(int***))) == NULL)	errorout(ERROR_MEM, "Could not create ***blks.");
	if ((puzzle->blks[ROW] = (int**) malloc(puzzle->length[ROW]*sizeof(int*))) == NULL)	errorout(ERROR_MEM, "Could not create **blks[ROW].");
	if ((puzzle->blks[COL] = (int**) malloc(puzzle->length[COL]*sizeof(int*))) == NULL)	errorout(ERROR_MEM, "Could not create **blks[COL].");

	int i, j;
	if ((cells = (Cell***) malloc(puzzle->length[COL]*sizeof(Cell**))) == NULL)	errorout(ERROR_MEM, "Could not create ***cells.");
	for (i = 0; i < puzzle->length[COL]; i++) {
		if ((cells[i] = (Cell**) malloc(puzzle->length[ROW]*sizeof(Cell*))) == NULL)	errorout(ERROR_MEM, "Could not create **cells.");
		for (j = 0; j < puzzle->length[ROW]; j++) {
			if ((cells[i][j] = (Cell*) malloc(sizeof(Cell))) == NULL)	errorout(ERROR_MEM, "Could not create *cells.");
			cells[i][j]->state = STATE_UNKN;
		}
	}

	for (i = 0; i < puzzle->length[ROW]; i++) {	//next L lines have row block lengths
		if (fgets(buffer, sizeof(buffer), fp) == NULL)	errorout(ERROR_BADFORMAT, "File ended unexpectedly (row list).");

		int arraySize;
		sscanf(buffer, " %d", &arraySize);		//careful with the case of having 0 blocks! the read line will be simply '0'
		if (arraySize > 0) {
			if ((puzzle->blks[ROW][i] = (int*) malloc(arraySize*sizeof(int))) == NULL)	errorout(ERROR_MEM, "Could not create *blks[ROW].");
		} else {	//special case: 0 means 1 block of 0 length (ie no blocks)
			if ((puzzle->blks[ROW][i] = (int*) malloc(sizeof(int))) == NULL)	errorout(ERROR_MEM, "Could not create *blks[ROW].");
			puzzle->blks[ROW][i][0] = 0;
		}

		int n, index = 0;
		for (n = index; !isspace(buffer[n]); n++) {	//"delete" first number, which determined arraySize
			buffer[n] = ' ';
			index = n + 1;
		}

		for (j = 0; j < arraySize; j++) {	//read arraySize blocks
			sscanf(buffer, " %d ", &(puzzle->blks[ROW][i][j]));	//get first integer
			for (n = index; !isspace(buffer[n]); n++) {	//replace all characters of the current word (ie the fetched integer) with whitespace
				buffer[n] = ' ';						//the goal of this is to have the "first" integer be the next one, so that with a single 
			}											//sscanf attribution we can read everything
			index = n + 1;	//store starting position of next word (ie next integer) to start on next whitespace-filling loop
		}
	}

	checkblankln(fp);	//get (3+L+1)th line - must be blank
	
	for (i = 0; i < puzzle->length[COL]; i++) {	//next C columns have col block lengths
		if (fgets(buffer, sizeof(buffer), fp) == NULL)	errorout(ERROR_BADFORMAT, "File ended unexpectedly (column list).");

		int arraySize;
		sscanf(buffer, "%d", &arraySize);		//careful with the case of having 0 blocks! the read line will be simply '0'
		if (arraySize > 0) {
			if ((puzzle->blks[COL][i] = (int*) malloc(arraySize*sizeof(int))) == NULL)	errorout(ERROR_MEM, "Could not create *blks[COL].");
		} else {
			if ((puzzle->blks[COL][i] = (int*) malloc(sizeof(int))) == NULL)	errorout(ERROR_MEM, "Could not create *blks[COL].");
			puzzle->blks[COL][i][0] = 0;
		}

		int n, index = 0;
		for (n = index; !isspace(buffer[n]); n++) {	//"delete" first number, which determined arraySize
			buffer[n] = ' ';
		}
		index = n + 1;

		for (j = 0; j < arraySize; j++) {	//read arraySize blocks
			sscanf(buffer, " %d ", &(puzzle->blks[COL][i][j]));	//get first integer
			for (n = index; !isspace(buffer[n]); n++) {	//replace all characters of the current word (ie the fetched integer) with whitespace
				buffer[n] = ' ';						//the goal of this is to have the "first" integer be the next one, so that with a single 
			}											//sscanf attribution we can read everything
			index = n + 1;	//store starting position of next word (ie next integer) to start on next whitespace-filling loop
		}
	}

	checkblankln(fp);	//get (3+L+1+C+1)th line - must be blank

	if (fgets(buffer, sizeof(buffer), fp) != NULL) {	//file must end after previous blank line
		errorout(ERROR_BADFORMAT, "File did not end when expected.");
	}

	if ((puzzle->line = (Line**) malloc(2*sizeof(Line*))) == NULL)	errorout(ERROR_MEM, "Could not create lines.");
	if ((puzzle->line[ROW] = (Line*) malloc(puzzle->length[ROW]*sizeof(Line))) == NULL)	errorout(ERROR_MEM, "Could not create rows.");
	for (i = 0; i < puzzle->length[ROW]; i++) {
		if ((puzzle->line[ROW][i].cells = (Cell**) malloc(puzzle->length[ROW]*sizeof(Cell*))) == NULL)	errorout(ERROR_MEM,"Could not create rows[i].cells.");
		for (j = 0; j < puzzle->length[COL]; j++) {
			puzzle->line[ROW][i].cells[j] = cells[i][j];
		}
	}
	if ((puzzle->line[COL] = (Line*) malloc(puzzle->length[COL]*sizeof(Line))) == NULL)	errorout(ERROR_MEM, "Could not create cols.");
	for (i = 0; i < puzzle->length[COL]; i++) {
		if ((puzzle->line[COL][i].cells = (Cell**) malloc(puzzle->length[COL]*sizeof(Cell*))) == NULL)	errorout(ERROR_MEM, "Could not create cols[i].cells.");
		for (j = 0; j < puzzle->length[ROW]; j++) {
			puzzle->line[COL][i].cells[j] = cells[j][i];
		}
	}

	return puzzle;
}

void LineCopy(Line* line1, Line* line2, int length) {
	int i;
	for (i = 0; i < length; i++) {
		line2[i] = line1[i];
	}
}

int getMinSumOfBlocksAndBlanks() {

}

int getLengthOfLargestBlock() {

}

/** stackline - "stacks" the right-most and left-most positionings of all blocks and compares them
 *	@param Puzzle* :	pointer to puzzle that's being worked on
 *  @param Line* :		pointer to line that needs stacking
 *	@param Stack* :		pointer to stack that 'interesting' perpendicular lines should be pushed to
 *	@noreturn
 *
 *	@verbose
 */
 
 //is there a point in having these special cases?
void stackline(Puzzle* puzzle, Line* line, Stack* stack, int coord) {
	int sum = getMinSumOfBlocksAndBlanks();
	int cap = getLengthOfLargestBlock();
	if (sum = 0) {	//means the whole line has blanks (special case)
		int i;
		for (i = 0; i < puzzle->length[coord]; i++) {
			line->cells[i]->state = STATE_BLNK;
		}
	} else
	if (puzzle->length[coord] == sum) {	//means the right-most arrangement is the same as the left-most: full solution is known (special case)
		//go through block lengths
	} else
	if (puzzle->length[coord] - sum < cap) {	//only situation in which stacking will do anything
		Line* left = (Line*) malloc(sizeof(Line));
		Line* right = (Line*) malloc(sizeof(Line));
		LineCopy(left, line, puzzle->length[coord]);
		LineCopy(right, line, puzzle->length[coord]);
	
		int i;
		for (i = 0; i < puzzle->length[coord]; i++) {
			if (left->cells[i]->state == right->cells[i]->state) {
				line->cells[i]->state = left->cells[i]->state;
			}
		}
	}

}

/** solveline - attempts to solve a line
 *	@param Puzzle* :	pointer to puzzle that's being worked on
 *	@param Line* :		pointer to line that needs solving
 *	@param Stack* :		pointer to perpendicular stack, to push interesting lines
 *	@return 			number of solved cells or -length[ROW]*length[COL] if line cannot be solved
 *
 *	@verbose			-length[ROW]*length[COL] is returned so that unsolvedcellcount is necessarily negative.
 */
int solveline(Puzzle* puzzle, Line* line, Stack* stack) {
	return 0;
}

/**	solve - finds every possible solution to 
 *	@param Puzzle* :	pointer to puzzle that needs solving
 *	@param Stack* : 	stack of rows of interest to solve
 *	@param Stack* : 	stack of columns of interest to solve
 *	@param int :		number of unkn cells in the current solution-attempt of the puzzle
 *	@noreturn
 *
 *	@verbose : uses solveline for every row in the stacks until they are empty or the puzzle has been solved. If the stacks are emptied before the solution is found, selects an unknown cell and tests both possible values. Does all this recursively until all solutions have been found.
 */
void solve(Puzzle* puzzle, Stack* stackRow, Stack* stackCol, int unsolvedCellCount) {
	while (!IsStackEmpty(stackRow) || !IsStackEmpty(stackCol) && unsolvedCellCount > 0) {
		unsolvedCellCount -= solveline(puzzle, Pop(stackCol), stackRow);
		unsolvedCellCount -= solveline(puzzle, Pop(stackRow), stackCol);
	}
	
	if (unsolvedCellCount > 0) {
		Puzzle* newPuzzle = (Puzzle*) malloc(sizeof(Puzzle));
		CopyPuzzle(newPuzzle, puzzle);

		int row = 0, col = 0;
		//get a random unknown cell, store coords		
		
		//set it to STATE_FULL		
		Push(stackRow, (void*) &newPuzzle->line[ROW][row]);
		Push(stackCol, (void*) &newPuzzle->line[COL][col]);
		solve(newPuzzle, stackRow, stackCol, unsolvedCellCount);
		
		//set it to STATE_BLNK
		Push(stackRow, (void*) &newPuzzle->line[ROW][row]);
		Push(stackCol, (void*) &newPuzzle->line[COL][col]);
		solve(newPuzzle, stackRow, stackCol, unsolvedCellCount);
	} else
	if (unsolvedCellCount = 0) {
		ClearStack(stackRow);	//could just not push perpendicular lines that are already
		ClearStack(stackCol);	//solved instead of clearing stacks here

		ExportSolution(puzzle);
	} else {
		//invalid solution, get out
	}
	
	return;
}

int main (int n, char** args) {
		if (n < 2) {
		printf("lol no\n");
		return -1;
	}
	
	Puzzle* puzzle = getPuzzle(args[1]);
	
	Stack* stackRow = CreateStack();
	Stack* stackCol = CreateStack();
	
	int i;
	for (i = 0; i < puzzle->length[ROW]; i++)	Push(stackRow, (void*) &puzzle->line[ROW][i]);
	for (i = 0; i < puzzle->length[COL]; i++)	Push(stackRow, (void*) &puzzle->line[COL][i]);
	
	solve(puzzle, stackRow, stackCol, puzzle->length[ROW] * puzzle->length[COL]);
	
	return 0;	
}
