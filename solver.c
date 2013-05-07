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
  * * * * * * * * * * * * * * * * x* * * * * */



//	time complexity of checking if a line is already solved: O(L) per line => O(L²*L) = O(L³) for the whole puzzle
			//(just loop through all cells and check none are UNKN)
//	memory complexity for storing if it is already solved:	O(L²) (for whole puzzle) with a time complexity of O(1)
			//(store a variable in each line struct, that gets updated as we solve each cell)




void ConditionalPush(Stack* stack, Line* line) {
//	if (line->unsolvedCells > 0) {
		Push(stack, (void*) line);
/*	} else {
		return;
	}*/
}


int getMinSumOfBlocksAndBlanksPrev(Line* line, int n);


int getBlockBounds(Line* line, int n, int* rmin, int* rmax, int length) {
	int i, j, num;
	int min = getMinSumOfBlocksAndBlanksPrev(line, n);					//inclusive min
	int max = length - getMinSumOfBlocksAndBlanks(line, n) - 1;			//inclusive max
	debp("%d = %d - %d - 1 for n=%d\n", max, length, getMinSumOfBlocksAndBlanks(line, n), n);
	//TODO in the middle of all this min/max detection: what about #'s that might belong to the next or previous block inside of the min-max zone?
	//and what if several other blocks can fit inside the min-max zone?
	/*falta delimitar maxs e mins pelas celulas cheias a uma distancia <= tamanho do bloco	!!!*/
	
//	printf("%d min %d max; %d n %d length\n", min,max,n,length);
	for (i = min, num = 0; i <= max; i++) {		//adjust min
		if (line->cells[i]->state == STATE_BLNK) {
			if (i - min < line->block[n].length) {
				min = i;
			} else {
				if (num++ > 0) return 0;	//means there's 2+ places where the block might be, nothing we can do, go to next block
			}
		}
	}
	if (num == 0) {		//block doesnt fit anywhere
//		debp("problum\n");
		return -1;
	}
	for (j = min; j <= max; j++) {					//adjust max
		if (line->cells[j]->state == STATE_BLNK) {
			max = j - 1;
		}
	}
	
	
	
	*rmin = min;
	*rmin = max;
	
	return 1;
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
int solveline(Puzzle* puzzle, Stack** stack, int x) {
	Line* line = Pop(stack[x]);
	int length = puzzle->length[x];
	int y = (x == ROW ? COL : ROW);
	int solvedCells = 0;

	int unknCount, fullCount, unknCountPost;
	int i, n;
	int min, max;
	int num;
	static int lne = 0;
	debp("line: %d\n", ++lne);
	for (n = 0, unknCount = 0, fullCount = 0, unknCountPost = 0; n < line->blockNum; n++) {
		if ((num = getBlockBounds(line, n, &min, &max, length)) == -1) return IMPOSSIBLE;
		if (!num)	continue;
		
		for (i = min + 1; i < max; i++) {
			switch (line->cells[i]->state) {
				 /* * * * * * * *
				  * It's a '#'	*
				  * * * * * * * */
				case STATE_FULL: {
					fullCount++;
					
					if (fullCount > line->block[n].length) {	//block too long, impossible
						return IMPOSSIBLE;
					}
					break;
				};



				 /* * * * * * * *
				  * It's a '-'	*
				  * * * * * * * */
				case STATE_BLNK: {
					//means the block all has to fit before this index, it's a new max. maybe it should've been detected in getMaxCellIndex?
					
					i = max + 1;	//since it should've been detected in getMaxCellIndex, program doesnt know what to do! skip to next block
					break;	//break unnecessary but looks good
				};



				 /* * * * * * * *
				  * It's a '?'	*
				  * * * * * * * */
				default: {
					if (unknCount == 0) {
						 /* * * * * * * * * * * * * * * * * * * * * * * * * * * * *
						  * The min cell was already #, the whole block is known. *
						  * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
						if (fullCount < line->block[n].length) {
							line->cells[i]->state = STATE_FULL;
							SolvedCell(i)
							fullCount++;
						} else
						if (fullCount == line->block[n].length) {
							line->cells[i]->state = STATE_BLNK;
							SolvedCell(i)
							i = max + 1;		//finished block, get out
						}
					} else {
						 /* * * * * * * * * * * * * * * * * * * * * * * * * * * * *
						  * The min cell was already #, the whole block is known. *
						  * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
						if (fullCount == 0) {
							unknCount++;
						} else {
							unknCountPost++;
						}
					}
					break;
				};
			}//end-switch
		}//end-cell-loop
		
		//TODO if we get here, check if the block needs solving (i == max, since when we skip cells it's set to max + 1)
		//and then based on fullcount, unkncount and unkncountpost, check which cells around fullcount we can determine to HAVE to belong to the block
		
	}//end-block-loop
	

	//TODO after all the block-based cell-filling, check for blanks that can be filled, although this might be doable inside the loop, we'll see
	
	return solvedCells;
}
#undef IMPOSSIBLE
#undef NextBlock
#undef SolvedCell
#undef Skip

//note: check to make sure that sum of blocks + blanks is <= line length in getPuzzle: if a line is not, just quit since puzzle is impossible


/** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
  * FreePuzzle: O(L²)		Just a macro function to clean up main. Frees memory allocated to puzzle.	*
  *																										*
  * @param Puzzle* :		puzzle to destroy															*
  *	@noreturn																							*
  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void FreePuzzle(Puzzle* puzzle) {	//O(Lr*Lc) + O(Lc) = O((Lr+1)*Lc) = O(Lr*Lc) = O(L²)
	int i, j;
	
	for (i = 0; i < puzzle->length[ROW]; i++){	//O(Lr)
		for (j = 0; j < puzzle->length[COL]; j++){	//O(Lc)
			free(puzzle->line[ROW][i].cells[j]);
		}
		free(puzzle->line[ROW][i].cells);
		free(puzzle->line[ROW][i].block);
	}
	for (i = 0; i < puzzle->length[COL]; i++){	//O(Lc)
		free(puzzle->line[COL][i].cells);
		free(puzzle->line[COL][i].block);
	}
	
	free(puzzle->line[ROW]);
	free(puzzle->line[COL]);
	free(puzzle->line);
	free(puzzle->length);
	free(puzzle);
}



/** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
  * getMinSumOfBlocksAndBlanks:	O(N)	Adds up the sizes of all blocks AFTER the current one, and then *
  * 									adds (blocknum - 1) for the mandatory amount of blanks.			*
  *																										*
  * @param Line* :						line to get sum of												*
  * @param int :						block index to start on											*
  *	@return int :						sum; will return a negative number if n is out of bounds		*
  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int getMinSumOfBlocksAndBlanks(Line* line, int n) {
	int i, count = 0;
	for (i = n; i < line->blockNum; i++) {	//O(N - n) = O(N) worst case
		count += line->block[i].length;
	}
	return count + (line->blockNum - n - 1);	//mandatory spaces, 1 for every block after n
}



/** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
  * getMinSumOfBlocksAndBlanksPrev:	O(N)	Adds up the sizes of all blocks BEFORE the current one, and then*
  * 										adds (blocknum - 1) for the mandatory amount of blanks.			*
  *																											*
  * @param Line* :							line to get sum of												*
  * @param int :							block index to start on											*
  *	@return int :							sum; 															*
  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int getMinSumOfBlocksAndBlanksPrev(Line* line, int n) {
	if (n == 0) return 0;
	
	int i, count = 0;
	for (i = n - 1; i >= 0; i--) {	//O(N - n) = O(N) worst case
		count += line->block[i].length;
	}
	return count + n;	//mandatory spaces, 1 for every block before n (which is = n)
}



/** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
  * getLengthOfLargestBlock: O(N)		Gets the size of the largest block in a line					*
  * 																									*
  * @param Line* :						line to get largest size from									*
  *	@return int :						length of largest block											*
  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int getLengthOfLargestBlock(Line* line) {
	int i, size;
	for (i = 0, size = 0; i < line->blockNum; i++) {
		if (line->block[i].length > size)	size = line->block[i].length;
	}
	return size;
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
  * PickCell: O(L²)		Gets the size of the largest block in a line									*
  * 																									*
  * @param Line* :		line to get largest size from													*
  *	@return int :		length of largest block															*
  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
Cell* PickCell(Puzzle* puzzle) {	//worst case: O(Lr*Lc) = O(L²)		, average is much much lower though
	int i, j;						//TODO still, L² is pretty bad.. should think about this
	for (i = 0; i < puzzle->length[ROW]; i++) {	//O(Lr*Lc)
		for (j = 0; j < puzzle->length[COL]; j++) {	//O(Lc)
			if (puzzle->line[ROW][i].cells[j]->state == STATE_UNKN) {
				return puzzle->line[ROW][i].cells[j];
			}
		}
	}
	return NULL;
}



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
/*		if (!IsStackEmpty(stack[ROW])) {
			line = Pop(stack[ROW]);
			
			if ((a = solveline(puzzle, line, stack[COL], ROW)) == -1)	return;	//return if impossibility is found
			
			if (line == &puzzle->line[ROW][0] || line == &puzzle->line[COL][0]) {	//special case
				//first blocks of perpendicular lines are known
			} else
			if (line == &puzzle->line[ROW][puzzle->length[ROW]-1] || line == &puzzle->line[COL][puzzle->length[COL]-1]) {	//special case
				//last blocks of perpendicular lines are known				
			}
			
		}*/
		if (!IsStackEmpty(stack[ROW])) {
			unsolvedCellCount -= solveline(puzzle, stack, ROW);
		}
		if (!IsStackEmpty(stack[COL])) {
			unsolvedCellCount -= solveline(puzzle, stack, COL);
		}
//		unsolvedCellCount -= a;
	}
	
//	ExportSolution(puzzle, stdout);

		debp("%d\n", unsolvedCellCount);

	if (unsolvedCellCount > 0) {
		if (stack[CELL] == NULL) stack[CELL] = CreateStack();
		
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
		
		pick->state = STATE_UNKN;
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
				unsolvedCellCount -= stackline(&puzzle->line[x][i], puzzle->length[x]);
			} else {
				return -1;
			}
		}
	}
	
	return unsolvedCellCount;
}



/** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
  * FreeStacks: O(1)		Just a macro function to clean up main. Frees memory allocated to stacks.	*
  *																										*
  * @param Stack** :		stack array to destroy														*
  *	@noreturn																							*
  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void FreeStacks(Stack** stack) {
	ClearStack(stack[ROW]);
	ClearStack(stack[COL]);
	free(stack[ROW]);
	free(stack[COL]);
	if (stack[CELL] != NULL) {
		ClearStack(stack[CELL]);
		free(stack[CELL]);
	}
	free(stack);
}


/** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
  * InitStacks: O(L)		Just a macro function to clean up main. Allocates memory to stacks and 		*
  *							pushes every row and column to its respective stack.						*
  *																										*
  * @param Puzzle* :		puzzle to get ROW|COL lengths from											*
  *	@return	Stack** :		stack array that was created and initialised								*
  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
Stack** InitStacks(Puzzle* puzzle) {	//O(2L)	= O(L)
	int i;

	Stack** stack = (Stack**) malloc((AXES+1)*sizeof(Stack*));
	stack[ROW] = CreateStack();
	for (i = 0; i < puzzle->length[ROW]; i++)	Push(stack[ROW], (void*) &puzzle->line[ROW][i]);	//O(Lr)
	stack[COL] = CreateStack();
	for (i = 0; i < puzzle->length[COL]; i++)	Push(stack[COL], (void*) &puzzle->line[COL][i]);	//O(Lc)
	stack[CELL] = NULL;	

	return stack;
}



//O(L²) + O(L) + O(L*N²) + O(TODO) + O(L²) + O(1) + O(1) =
//O(2L² + L + 2 + L*N² + TODO)

int main (int n, char** args) {
	if (n < 2) errorout(ERROR_ARGS, "No file name was given.");
	
	Puzzle* puzzle = getPuzzle(args[1]);	//O(L²)
	
	int unsolvedCellCount;
	if ((unsolvedCellCount = presolve(puzzle)) != 0) {	//O(L*N²)
		debp("%d\n", unsolvedCellCount);
		Stack** stack = InitStacks(puzzle);	//O(L)
		solve(puzzle, stack, unsolvedCellCount);		//O(TODO)
		FreeStacks(stack);	//O(1)
	}
	
	
	FreePuzzle(puzzle);	//O(L²)
	ExportSolution(NULL, NULL);	//O(1)
	
	return 0;
}

//maybe track amount of unsolved cells per line, and if a line is fully solved, don't push it onto the stack!

//possibly use a single stack, defining the axis that's currently being used in a static int in a short function, instead of repeatedly passing it along
//is it really necessary to have 2 stacks? they store line pointers, so it shouldn't matter if it's a row or a column, except for the issue with line length

/*
	QUESTIONS:
	//use a metasolve function that is mostly the same as solve so we don't have to carry the 'cellstack' through the first solve? most of the information is gathered with the first solve, there's no need for the extra memory usage since cellstack isnt even used there







*/
