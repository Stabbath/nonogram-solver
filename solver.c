#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stacks.h"
#include "solver.h"

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
  * TODO: Finish line solver				*
  *	 && Brute force change tracker/reverter *
  *	 && solver line/row pushing 			*
  * * * * * * * * * * * * * * * * * * * * * */

/*
	TODO: problem: lines that have a 0-length block will say that they have 0 blocks!
*/



//	time complexity of checking if a line is already solved: O(L) per line => O(L²*L) = O(L³) for the whole puzzle
			//(just loop through all cells and check none are UNKN)
//	memory complexity for storing if it is already solved:	O(L²) (for whole puzzle) with a time complexity of O(1)
			//(store a variable in each line struct, that gets updated as we solve each cell)


/*
int getBlockBounds(Line* line, int n, int* rmin, int* rmax, int length) {
	int min = getMinSumOfBlocksAndBlanksPrev(line, n);			//inclusive min
	int max = length - (getMinSumOfBlocksAndBlanks(line, n) - line->block[n].length) - 1;		//inclusive max	
	
	if (max - min + 1 <	line->block[n].length) {	//+1 because max is inclusive; if this is true the block can't fit anywhere
		return -1;
	}
	
	*rmin = min;
	*rmax = max;
	
	return 1;
}
*/




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




-#-#------
-#--#-----
-#---#----
-#----#---
-#-----#--
-#------#-
-#-------#
--#-#-----
--#--#----
--#---#---
--#----#--
--#-----#-
--#------#
---#-#----
---#--#---
---#---#--
---#----#-
---#-----#
----#-#---
----#--#--
----#---#-
----#----#
-----#-#--
-----#--#-
-----#---#
------#-#-
------#--#
-------#-#






NOTAS:

a secção que mete '-'s entre os maximos e os minimos nao está a funcionar muito bem. 
Se estiver comentada, há segfault nalgum sitio. 
Se não estiver comentada, não há segfault mas enche '-'s em sitios em que nao devia.




ideia:
possivel algoritmo, que talvez seja menos eficiente mas mais facil de implementar:
testamos todas as possiveis combinaçoes de posicoes para os blocos de uma linha. Vamos escrevendo (ou num clone da linha ou na propria linha mas depois desfazemos as mudanças se houverem problemas) os blocos na linha, e vamos testando os valores que já estavam na linha. Cada vez que chegamos ao fim da linha sem impossibilidades, guardamos a linha numa lista ou numa pilha. Se acontecer que para todas as posições de um bloco há uma certa célula que está sempre cheia, então ela tem de estar cheia.

Podemos guardar os mins e maxs de cada bloco à mesma, sendo que seriam descobertos por este método! E quando voltarmos à linha uma segunda vez, pode-se usar logo esses minimos e maximos como pontos de começo para voltar a encontrar todas as possiveis posições de todos os blocos da linha. Há outras optimizações que dá para inventar, e é mais fácil de implementar do que tudo o que já experimentámos já que não involve o que estavams a fazer, o que é basicamente um estudo matemático das condições das células para qualquer disposição geral da linha.

Em termos de complexidade.. O pior caso para isto significa 2 blocos de tamanho 1:
Para uma linha de exemplo de tamanho 10, completamente desconhecida:
??????????
#-#-------
#--#------
#---#-----
#----#----
#-----#---
#------#--
#-------#-
#--------#
-#-#------
-#--#-----
-#---#----
-#----#---
-#-----#--
-#------#-
-#-------#
--#-#-----
--#--#----
--#---#---
--#----#--
--#-----#-
--#------#
---#-#----
---#--#---
---#---#--
---#----#-
---#-----#
----#-#---
----#--#--
----#---#-
----#----#
-----#-#--
-----#--#-
-----#---#
------#-#-
------#--#
-------#-#
= 36 combinações

Como deves ter percebido isto é (n+1)n/2, com n = L - S, para comprimento de linha = L e S = soma de todos os blocos e espaços menos o primeiro bloco. Neste caso S = 2 => (10-2+1)*(10-2)/2 = 9*8/2 = 9*4 = 36

Por isso a complexidade para o pior caso, que é este que acabei de exemplificar só que para qualquer L, é O( (L-S+1)*(L-S)/2  ) = O( (L-1)*(L-2)/2 ) = O( (L²-3L+2)/2 ) = O(L²)

Quadrático em L é melhor até do que o stacklines que era O(L*N²) (mas continuamos a precisar desse para termos uma base decente para o puzzle)


Isto também está mais associado ao método que o professor tinha mencionado na aula de dúvidas da semana passada. Ele disse que em vez de olhar para as células como estávamos podiamos também olhar para o problema como sendo uma questão das diferentes combinações de posições dos blocos.

Que achas? Eu acho que é o melhor.


*/


void FreeLine(Line* line, int length) {
	int i;
	for (i = 0; i < length; i++) {
		free(line->cells[i]);
	}
	free(line->cells);
	free(line->block);
	free(line);
}


#define MODE_NO -1
#define MODE_GET 0
#define MODE_RESET 1

Line* MergeBlockPositions(Line* line, int length, int mode) {	//O(L)
	static Line* solution = NULL;
	int i;
	
	if (line == NULL) {			//after all merging is done, we need to fetch the solution and then clear alloc'd mem
		if (mode == MODE_GET) {
			return solution;
		} else
		if (mode == MODE_RESET) {
			FreeLine(solution, length);
			solution = NULL;
			return NULL;
		}
	}


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
			solution->cells[i]->state = line->cells[i]->state;
		}

	} else {	//if it's not the first time, then we have to update the solution based on mismatches with the new version of the line
		/* every cell in this version of the line that mismatches the previously held solution gets set to unknown */
		for (i = 0; i < length; i++) {
			if (line->cells[i]->state != solution->cells[i]->state) {
				solution->cells[i]->state = STATE_UNKN;
			}
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
void ExamineBlocks(Line* line, int n, int length, int start, Stack* cellstack) {
	int i, count = 0;
	
	/* fill this block's current position's cells */
	if (start > 0) {	//beginning blank
		if (line->cells[start-1]->state == STATE_FULL) return;
		if (line->cells[start-1]->state == STATE_UNKN) {
			line->cells[start-1]->state = STATE_BLNK;
			Push(cellstack, line->cells[start-1]);
			count++;
		}
	}
	for (i = start; i <= start + line->block[n].length - 1; i++) {
		if (line->cells[i]->state == STATE_BLNK) {
			while (count-- > 0) ((Cell*) Pop(cellstack))->state = STATE_UNKN;
			return;
		}
		if (line->cells[i]->state == STATE_UNKN) {
			line->cells[i]->state = STATE_FULL;
			Push(cellstack, line->cells[i]);
			count++;
		}
	}
	if (i < length - 1) {	//ending blank
		if (line->cells[start+1]->state == STATE_FULL) return;
		if (line->cells[start+1]->state == STATE_UNKN) {
			line->cells[start+1]->state = STATE_BLNK;
			Push(cellstack, line->cells[start+1]);
			count++;
		}
	}



	if (n < line->blockNum - 1) {	//not all blocks are in a position yet, go deeper
		/* test every possible position of the remaining blocks */
		int min = line->block[n+1].min;
		int max = line->block[n+1].max;
		int size = line->block[n+1].length;
		for (i = min; i <= MIN(max, length - 1) - size; i++) {	//test filling blocksize cells after i = min for every possible block start
			ExamineBlocks(line, n + 1, length, i, cellstack);
		}
	}
	else {	//all blocks are in a position, time to test them
		MergeBlockPositions(line, length, -1);
	}

	/* undo changes made to cells */
	while (count-- > 0) {
		((Cell*) Pop(cellstack))->state = STATE_UNKN;
	}
	
	return;
}




#define NextBlock 		n++;unknCount=0;fullCount=0;
#define SolvedCell(a)	solvedCells++;Push(stack[CELL],line->cells[a]);ConditionalPush(stack[y],&puzzle->line[y][a]);
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
  /*
  
  
  
  
  */
int solveline(Puzzle* puzzle, Stack** stack, int x) {
	Line* line = Pop(stack[x]);
	int length = puzzle->length[opAxis(x)];
	int solvedCells = 0;
	int i;
	
	MergeBlockPositions(line, length, -1);

	/* start recursive analysis of block positions */
	int min = line->block[0].min;
	int max = line->block[0].max;
	int size = line->block[0].length;
	for (i = min; i <= MIN(max, length - 1) - size; i++) {	//test filling blocksize cells after i = min for every possible block start
		Stack* st = CreateStack();
		ExamineBlocks(line, 0, length, i, st);
		while (!IsStackEmpty(st)) Pop(st);
		free(st);
	}

	Line* solution = MergeBlockPositions(NULL, length, MODE_GET);
	for (i = 0; i < length; i++) {
		line->cells[i]->state = solution->cells[i]->state;
	}
	MergeBlockPositions(NULL, length, MODE_RESET);
	
	return solvedCells;
}
#undef IMPOSSIBLE
#undef SolvedCell
#undef NextBlock


//note: check to make sure that sum of blocks + blanks is <= line length in getPuzzle: if a line is not, just quit since puzzle is impossible




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
void solve(Puzzle* puzzle, Stack** stack, int unsolvedCellCount) {
	while (!(IsStackEmpty(stack[ROW]) && IsStackEmpty(stack[COL])) && unsolvedCellCount > 0) {
		if (!IsStackEmpty(stack[ROW])) {
			unsolvedCellCount -= solveline(puzzle, stack, ROW);
		}
		if (!IsStackEmpty(stack[COL])) {
			unsolvedCellCount -= solveline(puzzle, stack, COL);
		}
	}
	
	ExportSolution(puzzle, stdout);

	if (unsolvedCellCount > 0) {
/*		if (stack[CELL] == NULL) stack[CELL] = CreateStack();
		
		int row = 0, col = 0;
		Cell* pick = PickCell(puzzle);	//O(L²) TODO but might be improved
		
		pick->state = STATE_FULL;
		Push(stack[ROW], (void*) &puzzle->line[ROW][row]);
		Push(stack[COL], (void*) &puzzle->line[COL][col]);
		solve(puzzle, stack, unsolvedCellCount - 1);
		while (!IsStackEmpty(stack[CELL])) {
			((Cell*) Pop(stack[CELL]))->state = STATE_UNKN;
		}
		
		pick->state = STATE_BLNK;
		Push(stack[ROW], (void*) &puzzle->line[ROW][row]);
		Push(stack[COL], (void*) &puzzle->line[COL][col]);
		solve(puzzle, stack, unsolvedCellCount - 1);
		while (!IsStackEmpty(stack[CELL])) {
			((Cell*) Pop(stack[CELL]))->state = STATE_UNKN;
		}
		
		pick->state = STATE_UNKN;*/
	} else
	if (unsolvedCellCount == 0) {
		ClearStack(stack[ROW]);	//could just not push perpendicular lines that are already
		ClearStack(stack[COL]);	//solved instead of clearing stacks here

		ExportSolution(puzzle, stdout);
	} else {
		//invalid solution, get out
	}
	
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
  *     O(N)  +  max( (O(L), O(N*B), O(N*F(B-L+S)) )										*
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
	
	int i, ret = 0;
	if (sum == 0) {	//O(L)
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
		 * Special case: If sum is 0, then the whole line is blank!  *
		 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
		for (i = 0; i < length; i++) {	//O(L)
			if (line->cells[i]->state == STATE_UNKN) {
				line->cells[i]->state = STATE_BLNK;
				ret++;
			} else 
			if (line->cells[i]->state == STATE_FULL) {
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
				if (line->cells[l]->state == STATE_UNKN) {
					line->cells[l]->state = STATE_FULL;
					ret++;
				} else
				if (line->cells[l]->state == STATE_BLNK) {
					return IMPOSSIBLE;	//impossible
				}
			}
			
			if (l < length) {
				if (line->cells[l]->state == STATE_UNKN) {
					line->cells[l]->state = STATE_BLNK;
					ret++;
				} else
				if (line->cells[l]->state == STATE_FULL) {	//next cell must be blnk
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
					if (line->cells[i]->state == STATE_BLNK) {
						return IMPOSSIBLE;								//impossible
					} else
					if (line->cells[i]->state == STATE_UNKN) {
						line->cells[i]->state = STATE_FULL;
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
	int i, x;
	for (x = ROW; x < AXES; x++) {	//O((Lr+Lc)*N²) = O(2*L*N²) = O(L*N²)
		for (i = 0; i < puzzle->length[x]; i++) {	//O(Lr|Lc * N²) = O(L*N²)
			if (unsolvedCellCount > 0) {
				unsolvedCellCount -= stackline(&puzzle->line[x][i], puzzle->length[opAxis(x)]);
			} else {
				return -1;
			}
		}
	}

	return unsolvedCellCount;
}


//O(L²) + O(L) + O(L*N²) + O(TODO) + O(L²) + O(1) + O(1) =
//O(2L² + L + 2 + L*N² + TODO) =
//O(TODO)

int main (int num, char** args) {
	if (num < 2) errorout(ERROR_ARGS, "No file name was given.");
	
	if (strlen(args[1]) >= MAXPATH) errorout(ERROR_ARGS, "Filename too long.");
	
	Puzzle* puzzle = getPuzzle(args[1]);	//O(L²)
	
	int unsolvedCellCount;
	if ((unsolvedCellCount = presolve(puzzle)) != 0) {	//O(L*N²)
		Stack** stack = InitStacks(puzzle);	//O(L)
		solve(puzzle, stack, unsolvedCellCount);		//O(TODO)
		FreeStacks(stack);	//O(1)
	} else {
		ExportSolution(puzzle, stdout);
	}
	
	FreePuzzle(puzzle);	//O(L²)
	
	ExportSolution(NULL, NULL);	//O(1)
	
	return 0;
}

//maybe track amount of unsolved cells per line, and if a line is fully solved, don't push it onto the stack!

//possibly use a single stack, defining the axis that's currently being used in a static int in a short function, instead of repeatedly passing it along
//is it really necessary to have 2 stacks? they store line pointers, so it shouldn't matter if it's a row or a column, except for the issue with line length
