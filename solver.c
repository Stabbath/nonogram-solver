#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stacks.h"
#include "solver.h"
#include "solverstructs.c"


int checkpuzzle(Puzzle* puzzle);
int checkline(Line* line, int length);

//#define ADVANCED

 /* * * * * * * * * * * * * * * * * * * * * *
  * Complexity variables: 					*
  *	N - number of blocks in line 			*
  *	L - length of line (r|c => row|column)	*
  *	B - length of block						*
  * S - sum of blocks + blanks in line		*
  * --------------------------------------- *
  * These usually refer to puzzle-wide avgs.*
  * * * * * * * * * * * * * * * * * * * * * */

 /* * * * * * * * * * * * * * * * * * * * * *
  * TODO: Fix ExamineBlocks					*
  *	 && Brute force change tracker/reverter *
  *	 && solver line/row pushing 			*
  * * * * * * * * * * * * * * * * * * * * * */

//note: check to make sure that sum of blocks + blanks is <= line length in getPuzzle: if a line is not, just quit since puzzle is impossible



/*
n = 1, L = 6
possibilidades = 6
n = 2, L = 6
possibilidades = +4 +3 +2 +1 = 10
#-#---	
#--#--
#---#-
#----#
n = 3. L = 6
possibilidades = 4
#-#-#-
#-#--#
#--#-#
-#-#-#

n = 1, L = 8
possibilidades = 8
n = 2, L = 8
possibilidades = 10
#-#---
#--#--
#---#-
#----#
-#-#--
-#--#-
-#---#
--#-#-
--#--#
---#-#
n = 3. L = 8
possibilidades = 20
#-#-#---
#-#--#--
#-#---#-
#-#----#
#--#-#--
#--#--#-
#--#---#
#---#-#-
#---#--#
#----#-#
-#-#-#--
4+3+2+1+3+2+1+2+1

??????????
#-#-#-----	//6
#-#--#----
#-#---#---
#-#----#--
#-#-----#-
#-#------#
#--#-#----	//5
#--#--#---
#--#---#--
#--#----#-
#--#-----#
#---#-#---	//4
#---#--#--
#---#---#-
#---#----#
#----#-#--	//3
#----#--#-
#----#---#
#-----#-#-	//2
#-----#--#
#------#-#	//1

-#-#-#----	//+5+4+3+2+1
-#-#--#---
-#-#---#--
-#-#----#-
-#-#-----#
-#--#-#---
-#--#--#--
-#--#---#-
-#--#----#
-#---#-#--

--#-#-#---	//+4+3+2+1
---#-#-#--	//+3+2+1
----#-#-#-	//+2+1
-----#-#-#	//+1


Pior caso implica obviamente que todos os blocos têm tamanho mínimo = 1.
Por esta razao, S = N*2 - 1 //N blocos + N espaços - 1 espaço
Sp = soma dos blocos anteriores
Sn = soma dos blocos seguintes, incluindo o proprio bloco


posições possíveis para o 1º bloco: L-(S-1) = L-(N*2-2) = L - N*2 + 2
posições possíveis para o 2º bloco: L - Sp - Sn = L - S = L - N*2 - 1
posicoes possiveis para o 3º bloco: L - S = L - N*2 - 1
... nº bloco: L - S 

F = (L - N*2 - 1)^N <- need to figure out what the exact expression is

extremos:
    dF/dL = 0


NOTAS:

a secção que mete '-'s entre os maximos e os minimos nao está a funcionar muito bem. 
Se estiver comentada, há segfault nalgum sitio. 
Se não estiver comentada, não há segfault mas enche '-'s em sitios em que nao devia.




ideia:
possivel algoritmo, que talvez seja menos eficiente mas mais facil de implementar:
testamos todas as possiveis combinaçoes de posicoes para os blocos de uma linha. Vamos escrevendo (ou num clone da linha ou na propria linha mas depois desfazemos as mudanças se houverem problemas) os blocos na linha, e vamos testando os valores que já estavam na linha. Cada vez que chegamos ao fim da linha sem impossibilidades, guardamos a linha numa lista ou numa pilha. Se acontecer que para todas as posições de um bloco há uma certa célula que está sempre cheia, então ela tem de estar cheia.

Podemos guardar os mins e maxs de cada bloco à mesma, sendo que seriam descobertos por este método! E quando voltarmos à linha uma segunda vez, pode-se usar logo esses minimos e maximos como pontos de começo para voltar a encontrar todas as possiveis posições de todos os blocos da linha. Há outras optimizações que dá para inventar, e é mais fácil de implementar do que tudo o que já experimentámos já que não involve o que estavams a fazer, o que é basicamente um estudo matemático das condições das células para qualquer disposição geral da linha.

Como deves ter percebido isto é (n+1)n/2, com n = L - S, para comprimento de linha = L e S = soma de todos os blocos e espaços menos o primeiro bloco. Neste caso S = 2 => (10-2+1)*(10-2)/2 = 9*8/2 = 9*4 = 36

Por isso a complexidade para o pior caso, que é este que acabei de exemplificar só que para qualquer L, é O( (L-S+1)*(L-S)/2  ) = O( (L-1)*(L-2)/2 ) = O( (L²-3L+2)/2 ) = O(L²)

Quadrático em L é melhor até do que o stacklines que era O(L*N²) (mas continuamos a precisar desse para termos uma base decente para o puzzle)


Isto também está mais associado ao método que o professor tinha mencionado na aula de dúvidas da semana passada. Ele disse que em vez de olhar para as células como estávamos podiamos também olhar para o problema como sendo uma questão das diferentes combinações de posições dos blocos.

Que achas? Eu acho que é o melhor.
*/


#define MODE_GET 0
#define MODE_RESET 1
#define MODE_TEST 2

Line* MergeBlockPositions(Line* line, int length, int mode) {	//O(L)
	static Line* solution = NULL;
	int i;
	
	switch (mode) {
		case MODE_RESET: {
			for (i = 0; i < length; i++) {
				free(solution->cells[i]);
			}
			free(solution->cells);
			free(solution->block);
			free(solution);
			solution = NULL;
			break;
		}
		case MODE_GET: {
			break;
		}
		case MODE_TEST: {
			if (solution == NULL) {		//line is brand new
				/* clone the line */
				solution = (Line*) malloc(sizeof(Line));
				solution->blockNum = line->blockNum;
				solution->block = (Block*) malloc(solution->blockNum*sizeof(Block));
				for (i = 0; i < solution->blockNum; i++) {
					solution->block[i].length = line->block[i].length;
					solution->block[i].min = line->block[i].min;
					solution->block[i].max = line->block[i].max;
				}

				solution->cells = (Cell**) malloc(length*sizeof(Cell*));
				for (i = 0; i < length; i++) {
					solution->cells[i] = (Cell*) malloc(sizeof(Cell));
					SetState(solution->cells[i], GetState(line->cells[i]));
				}
			
			} else {	//if it's not the first time, then we have to update the solution based on mismatches with the new version of the line
				/* every cell in this version of the line that mismatches the previously held solution gets set to unknown */
				for (i = 0; i < length; i++) {
					if (GetState(line->cells[i]) != GetState(solution->cells[i])) {
						SetState(solution->cells[i], STATE_UNKN);
					}
				}
			}
			break;
		}
	}
	
	return solution;
}


/** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
  * ExamineBlocks:			Tests every possible block position based on previous block positions, and	*
  *							attempts to discover implicit cells.										*
  *																										*
  * @param Line* :			line whose blocks we're examining											*
  * @param int :			block index we're on														*
  * @param int :			length of line																*
  * @param int :			first cell of the current test position for this block (inclusive)			*
  * @param int :			last cell of the current test position for this block (inclusive)			*
  *	@noreturn :																							*
  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void ExamineBlocks(Line* line, int n, int length, int start, Stack* cellstack, int prevblockend) {
	int i, count = 0;
	Cell** cells = line->cells;
	
	/* fill blanks before position until the prev block's ending index*/
	if (start > 0) {	//beginning blank
		if (cells[start-1]->state == STATE_FULL) return;
		if (cells[start-1]->state == STATE_UNKN) {
			cells[start-1]->state = STATE_BLNK;
			Push(cellstack, cells[start-1]);
			count++;
		}
		for (i = start - 2; i > prevblockend; i--) {		//fill blanks between this block and previous one
			if (cells[i]->state == STATE_UNKN) {
				cells[i]->state = STATE_BLNK;
				Push(cellstack, cells[i]);
				count++;
			} else
			if (cells[i]->state == STATE_FULL) {
				while (count-- > 0) ((Cell*) Pop(cellstack))->state = STATE_UNKN;
				return;
			}
		}
	}

	/* fill this block's current position's cells */
	for (i = start; i <= start + line->block[n].length - 1; i++) {
		if (cells[i]->state == STATE_BLNK) {
			while (count-- > 0) ((Cell*) Pop(cellstack))->state = STATE_UNKN;
			return;
		}
		if (cells[i]->state == STATE_UNKN) {
			cells[i]->state = STATE_FULL;
			Push(cellstack, line->cells[i]);
			count++;
		}
	}

	/* fill blanks after position */	
	if (i < length) {	//terminating blank
		if (cells[i]->state == STATE_FULL) {
			while (count-- > 0) ((Cell*) Pop(cellstack))->state = STATE_UNKN;
			return;
		}
		if (cells[i]->state == STATE_UNKN) {
			cells[i]->state = STATE_BLNK;
			Push(cellstack, cells[i]);
			count++;
		}
		if (n == line->blockNum - 1) {
			for (/*dont change i*/; i < length; i++) {		//fill blanks between this block and the end, since it's the last block
				if (cells[i]->state == STATE_UNKN) {
					cells[i]->state = STATE_BLNK;
					Push(cellstack, cells[i]);
					count++;
				} else
				if (cells[i]->state == STATE_FULL) {
					while (count-- > 0) ((Cell*) Pop(cellstack))->state = STATE_UNKN;
					return;
				}
			}
		}
	}
	
	if (n < line->blockNum - 1) {	//not all blocks are in a position yet, go deeper
		/* test every possible position of the remaining blocks */
		int j;
		int min = line->block[n+1].min;
		int max = line->block[n+1].max;
		int size = line->block[n+1].length;
		for (j = min; j <= max - size + 1; j++) {	//test filling blocksize cells after i = min for every possible block start
			ExamineBlocks(line, n + 1, length, j, cellstack, i  - 1);
		}
	} else {	//all blocks are in a position, time to test them
		MergeBlockPositions(line, length, MODE_TEST);
	}
	
	/* undo changes made to cells */
	while (count-- > 0) ((Cell*) Pop(cellstack))->state = STATE_UNKN;	
	return;
}




#define IMPOSSIBLE		puzzle->length[ROW]*puzzle->length[COL]
/** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
  * solveline:				Solves cells based on the line's native characteristics, ie block number 	*
  * O(TODO)					and block sizes, COUPLED WITH all information that's already been gathered	*
  *							regarding identified cells.													*
  *																										*
  * @param Puzzle* :		puzzle we're working on														*
  * @param Line* :			line to solve																*
  * @param Stack** :		support stack array															*
  * @param int :			ROW|COL - coordinate of this line											*
  *	@return int :			number of solved cells, or -1 if an impossibility was discovered.			*
  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int solveline(Puzzle* puzzle, Stack** stack, Stack* cellstack, int x) {
	Line* line = Pop(stack[x]);
//	if (line->unsolvedCells == 0) return 0;		//don't test solved lines
	
	int length = puzzle->length[!x];
	int solvedCells = 0;
	int i;
	
	/* start recursive analysis of block positions */
	int highestMin = line->block[0].max - line->block[0].length + 1;
	for (i = line->block[0].min; i <= highestMin; i++) {	//test filling blocksize cells after i = min for every possible block start
		Stack* st = CreateStack();
		ExamineBlocks(line, 0, length, i, st, -1);
		while (!IsStackEmpty(st)) ((Cell*) Pop(st))->state = STATE_UNKN;	//reset just in case
		free(st);
	}
	
	
	Line* solution = MergeBlockPositions(NULL, length, MODE_GET);
	if (solution == NULL) return IMPOSSIBLE;	//NULL means we didn't succeed at all in the previous loop
	for (i = 0; i < length; i++) {
		if (line->cells[i]->state != solution->cells[i]->state) {
			if (line->cells[i]->state == STATE_UNKN) {
				line->cells[i]->state = solution->cells[i]->state;
				line->cells[i]->row->unsolvedCells--;
				line->cells[i]->col->unsolvedCells--;
				solvedCells++;
				Push(cellstack,line->cells[i]);
				ConditionalPush(stack[!x],&puzzle->line[!x][i]);
			} else {
				MergeBlockPositions(NULL, length, MODE_RESET);
				return IMPOSSIBLE;	//can this even get this far without detection? better safe than sorry though!
			}
		}
	}
	
	
	MergeBlockPositions(NULL, length, MODE_RESET);
	return solvedCells;
}
#undef IMPOSSIBLE
#undef SolvedCell




/** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
  * solve: O(TODO)			Solves cells implicitly based on the line's native characteristics			*
  * 						ie block number and block sizes.											*
  *																										*
  * @param Puzzle* :		puzzle to solve																*
  * @param Stack** :		stack array	to use															*
  * @param int :			number of unsolved cells													*
  *	@noreturn																							*
  * 																									*
  * @verbose : 				uses solveline for every row in the stacks until they are empty or the 		*
  * 						puzzle has been solved. If the stacks are emptied before the solution is 	*
  *							found, selects a STATE_UNKN cell and tests it for both values recursively,	*
  *							after pushing the row and column that the selected cell belongs to onto the	*
  *							solver stack. Does this until all solutions have been found.				*
  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void solve(Puzzle* puzzle, Stack** stack, Stack* cellstack, int unsolvedCellCount) {
	while (!(IsStackEmpty(stack[ROW]) && IsStackEmpty(stack[COL])) && unsolvedCellCount > 0) {
		if (!IsStackEmpty(stack[ROW])) {
			unsolvedCellCount -= solveline(puzzle, stack, cellstack, ROW);
		}
		if (!IsStackEmpty(stack[COL])) {
			unsolvedCellCount -= solveline(puzzle, stack, cellstack, COL);
		}
	}

	if (unsolvedCellCount > 0) {
		Stack* nextcellstack = CreateStack();
		
		int row, col;
		Cell* pick = PickCell(puzzle, &row, &col);	//O(L²) TODO but might be improved
		Cell* cell;
		
		puzzle->line[ROW][row].unsolvedCells--;
		puzzle->line[COL][col].unsolvedCells--;

		pick->state = STATE_FULL;
		Push(stack[ROW], (void*) &puzzle->line[ROW][row]);
		Push(stack[COL], (void*) &puzzle->line[COL][col]);
		solve(puzzle, stack, nextcellstack, unsolvedCellCount - 1);
		while (!IsStackEmpty(nextcellstack)) {
			cell = (Cell*) Pop(nextcellstack);
			cell->state = STATE_UNKN;
			cell->row->unsolvedCells++;
			cell->col->unsolvedCells++;
		}
		
		pick->state = STATE_BLNK;
		Push(stack[ROW], (void*) &puzzle->line[ROW][row]);
		Push(stack[COL], (void*) &puzzle->line[COL][col]);
		solve(puzzle, stack, nextcellstack, unsolvedCellCount - 1);
		while (!IsStackEmpty(nextcellstack)) {
			cell = (Cell*) Pop(nextcellstack);
			cell->state = STATE_UNKN;
			cell->row->unsolvedCells++;
			cell->col->unsolvedCells++;
		}
		
		pick->state = STATE_UNKN;
		
		ClearStack(nextcellstack);
		free(nextcellstack);
	} else
	if (unsolvedCellCount == 0) {
		if (checkpuzzle(puzzle)) ExportSolution(puzzle, NULL);
	} else {
		//invalid solution, get out
	}
	
	ClearStack(stack[ROW]);	//just in case
	ClearStack(stack[COL]);	//

	
	return;
}



/** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
  * stackline: O(N²)		Solves cells implicitly based on the line's native 							*
  *							characteristics, ie block number and block sizes.							*
  *																										*
  * NOTE: this routine will never provide new information if used more than once for a single line.		*
  *																										*
  * @param Line* :			line to solve																*
  * @param int :			length of this line 														*
  *	@return int :			number of solved cells, or -1 if an impossibility was discovered.			*
  * 																									*
  * @verbose : TODO																						*
  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#define IMPOSSIBLE	-1
int stackline(Line* line, int length) {
 /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
  * NOTE: With N*B, B is the average block size, meaning N*B is the sum of all block lengths*
  *																							*
  *	   F(x) is a function which returns y = hitrate*x, 0 <= hitrate <= 1 because only the	*
  * blocks for which there is a superposition will lead to O(B-L+S). Worst case, ie every 	*
  * block has a non-null superposition, means:  N*B = L-1  ,  F(N*(B-L+S)) = N*(B-L+S) =    *
  *	=  L-1 - N*L + N*S																		*
  *																							*
  *     O(N)  +  max( (O(L), O(N*B), O(N*F(B-L+S)) )											*
  * =   O(N + L + L-1 + L-1-N*L+N*S)				, N*B = L-1; -1 are neglibile			*
  * =   O(N + 3L - N*L + N*S)						, S = N*B + N-1	= L-1 + N-1				*
  * =   O(N + (3-N)*L + N*(L+N-2))															*
  * =   O(N + (3-N)*L + N*L + N² - 2*N)														*
  * =   O(N² + 3*L - N)																		*
  * =   O(N² + 3*L)									, dN²/dL >> d(3*L)/dL					*
  * =   O(N²)																				*
  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
//alternative:
 /*																							*
  *     O(N)  +  max( (O(L), O(N*B), O(N*F(B-L+S)) )										*
  * =   O(N + L + N*B + L-1 - N*L + N*S)			, -1 is neglibile						*
  * =   O((N+1)*B - (N-2)*L + N*S)					, S = N*B + N-1							*
  * =   O((N+1)*B - (N-2)*L + N*(N*B+N-1))			, 										*
  * =   O((N²+N+1)*B - (N-2)*L + N²-N 				, L = N*B + 1 							*
  * =   O((N²+N+1)*B - (N-2)*(N*B+1) + N²-N													*
  * =   O(2*N²*B - N*B + B - N + 2 + N² - N)												*
  * =   O(N²*B - N*B + B + N² - N)															*
  * =   O(N²*B + N²)																		*
  * =   O(N²*(B+1))                                 , dN²/dL >> d(B+1)/dL                   *
  * =   O(N²)           	 	                                               	            *
  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	int sum = getMinSumOfBlocksAndBlanks(line, 0);	//O(N) 
	int cap = getLengthOfLargestBlock(line);	//O(N)

	if (sum > length) {
		return IMPOSSIBLE;	//impossible
	}
	
	Cell** cells = line->cells;
	int i, ret = 0;
	if (sum == 0) {	//O(L)
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
		 * Special case: If sum is 0, then the whole line is blank!  *
		 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
		for (i = 0; i < length; i++) {	//O(L)
			if (cells[i]->state == STATE_UNKN) {
				cells[i]->state = STATE_BLNK;
				ret++;
			} else 
			if (cells[i]->state == STATE_FULL) {
				return IMPOSSIBLE;	//impossible
			}
		}
		return ret;
	} else
	if (length == sum) {	//O(N*B)
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
		 * Special case: If sum is length, then the whole line is known! *
		 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
		int i, j, l = 0;
		for (i = 0; i < line->blockNum; i++, l++) {	//O(N)
			for (j = 0; j < line->block[i].length; j++, l++) {	//O(B)
				if (cells[l]->state == STATE_UNKN) {
					cells[l]->state = STATE_FULL;
					ret++;
				} else
				if (cells[l]->state == STATE_BLNK) {
					return IMPOSSIBLE;	//impossible
				}
			}
			
			if (l < length) {
				if (cells[l]->state == STATE_UNKN) {
					cells[l]->state = STATE_BLNK;
					ret++;
				} else
				if (cells[l]->state == STATE_FULL) {	//next cell must be blnk
					return IMPOSSIBLE;		//impossible
				}
			}
		}
		return ret;
	} else
	if (length - sum < cap) {	//O(N*F(B-L+S))				//only situation in which stacking will do anything
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
		 *  Using simple math we can identify which cells must be filled and which remain unknown. The first *
		 * length-sum cells are unaffected because length-sum is the difference between the first cell of the*
		 * left-most and right-most positions of the any block in the line. Then, blocksize-(length-sum) is  *
		 * the amount of cells which must be filled if ANY cells at all are to be filled (ie if there is a   *
		 * superposition of the leftmost and rightmost possibilities). If no cells can be filled, we skip    *
		 * blocksize cells to account for that block's placement, otherwise we skip another length-sum to the*
		 * right of the block. Also, if length - sum < cap, we know there is no superposition for any block. *
		 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
		int n = 0;
		i = length - sum;												//the first (length - sum) cells are unaffected, skip them
		while (n < line->blockNum) {	//O(N*F(B-L+S))
			int limit = i + line->block[n].length - (length - sum);		//the next (blocksize - (linelength - sum)) cells are full
			if (line->block[n].length > length - sum) {					//is there a superposition of this block?
				for (; i < limit; i++) {	//O(B-L+S) only if b > l - s, else O(1)
					if (cells[i]->state == STATE_BLNK) {
						return IMPOSSIBLE;								//impossible
					} else
					if (cells[i]->state == STATE_UNKN) {
						cells[i]->state = STATE_FULL;
						ret++;
					}
				}
				i += length - sum;										//the next (length - sum) cells are unaffected
			} else {	//O(1)
				i += line->block[n].length;								//skip blocksize cells!
			}
			
			i++;														//1 more cell, for the mandatory space
			n++;														//next block
		}
		return ret;
	}	
	return 0;
}
#undef IMPOSSIBLE



/** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
  * presolve: O(L*N²)		Calls stackline on every row and on every column once to solve	 			*
  *							the easiest cells, to pave the way for the fuller linesolver.				*
  *																										*
  * @param Puzzle* :																					*
  *	@return int :			number of unsolved cells left in the puzzle									*
  * 																									*
  * @verbose : 				  Solves cells implicitly based on each line's native characteristics, ie	*
  * 						block numbers and block sizes. Basically fetches all of the basic "easy"	*
  *							information for a puzzle before the program is allowed to go into the more	*
  *							complete algorithm.	For more information check the stackline function.		*
  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int presolve(Puzzle* puzzle) {	//O(L*N²)
	int unsolvedCellCount = puzzle->length[ROW] * puzzle->length[COL];
	int i, x, buf;
	for (x = ROW; x < AXES; x++) {	//O((Lr+Lc)*N²) = O(2*L*N²) = O(L*N²)
		for (i = 0; i < puzzle->length[x]; i++) {	//O(Lr|Lc * N²) = O(L*N²)
			if (unsolvedCellCount > 0) {
				buf = stackline(&puzzle->line[x][i], puzzle->length[!x]);
				unsolvedCellCount -= buf;
				puzzle->line[x][i].unsolvedCells -= buf;
			} else {
				return unsolvedCellCount;
			}
		}
	}

	return unsolvedCellCount;
}




int getSumOfBlocks(Line* line) {
	if (line->block[0].length == 0) return 0;
	
	int i, count = 0;
	for (i = 0; i < line->blockNum; i++) {	//O(N - n) = O(N) worst case
		count += line->block[i].length;
	}
	return count;
}

int checkline(Line* line, int length) {
	if (line->block[0].length == 0) return 1;	//blank lines are correctly always filled

	int sum = getSumOfBlocks(line);
	int count = 0;
	int streak = 0;
	int j;
	Stack* streakStack = CreateStack();
	for (j = 0; j < length; j++) {
		if (line->cells[j]->state == STATE_FULL) {
			count++;
			streak++;
		}
		if (line->cells[j]->state == STATE_BLNK || j == length - 1) {
			if (streak != 0) {
				int* tmp = malloc(sizeof(tmp));
				*tmp = streak;
				Push(streakStack, (void*) tmp);
			}
			streak = 0;
		}
	}
	if (sum != count) return 0;	//number of # is different from what was expected
	
	int n;
	int success = 1;
	int* tmp;
	for (n = line->blockNum - 1; n >= 0; n--) {
		if (IsStackEmpty(streakStack)) {	//means there were less blocks than there were supposed to be
			success = 0;
			break;
		}
		
		tmp = (int*) Pop(streakStack);
		if (line->block[n].length != *tmp) {	//means this block had the wrong length
			success = 0;
		}
		
		free(tmp);
	}
	if (!IsStackEmpty(streakStack)) {
		success = 0;	//means there were more blocks than there were supposed to be
		while (!IsStackEmpty(streakStack)) {
			free(Pop(streakStack));
		}
	}
	free(streakStack);
	
	return success;
}

int checkpuzzle(Puzzle* puzzle) {
	int i, x;
	Line* lines;
	for (x = ROW; x < AXES; x++) {
		lines = puzzle->line[x];
		for (i = 0; i < puzzle->length[x]; i++) {
			if (!checkline(&lines[i], puzzle->length[!x])) return 0;
		}
	}
	
	return 1;
}


//O(L²) + O(L) + O(L*N²) + O(TODO) + O(L²) + O(1) + O(1) =
//O(2L² + L + 2 + L*N² + TODO) =
//O(TODO)

int main(int num, char** args) {
	if (num < 2) errorout(ERROR_ARGS, "No file name was given.");
	
	if (strlen(args[1]) >= MAXPATH) errorout(ERROR_ARGS, "Filename too long.");
	
	Puzzle* puzzle = getPuzzle(args[1]);	//O(L²)
	
	int i, j;	//store lines each cell belongs to inside of the cell structs
	for (i = 0; i < puzzle->length[ROW]; i++) {
		for (j = 0; j < puzzle->length[COL]; j++) {
			puzzle->line[ROW][i].cells[j]->row = &puzzle->line[ROW][i];
			puzzle->line[COL][j].cells[i]->col = &puzzle->line[COL][j];
		}
	}
	
	int unsolvedCellCount = presolve(puzzle);
	if (unsolvedCellCount > 0) {	//O(L*N²)
		Stack** stack = InitStacks(puzzle);	//O(L)
		solve(puzzle, stack, NULL, unsolvedCellCount);		//O(TODO)
		FreeStacks(stack);	//O(1)
	} else
	if (unsolvedCellCount == 0) {
		ExportSolution(puzzle, NULL);
	}
	
	ExportSolution(NULL, puzzle->name);	//O(1)
	
	FreePuzzle(puzzle);	//O(L²)
	
	return 0;
}
