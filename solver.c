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


//NOTE: these functions can be merged, with 'blockid' being incremented based on which check
//we want. Would also allow for checks with blockid + n, for any n
//also, there is no need for the 'i' var

/*can all remaining blocks and spaces (including the current block)
  fit after the current cell?*/
int checkA(Line* line, int blockid, int unkns, int index, int length) {	//O(N-i), with i being the current block. Worst case, i=0 -> O(N)
	int i, count = 0;
	for (i = blockid; i < line->blockNum; i++) count += 1 + line->block[i].length;	//O(N-i)
	count--; //because we considered 1 space too many
	
	if (count < (length - 1) - index) {	//means we can't fit all the blocks and spaces
		return 0;
	}
	return 1;
}

/*can all remaining blocks and spaces (EXCLUDING the current block)
  fit after the current cell?*/
int checkAtilde(Line* line, int blockid, int unkns, int index, int length) {
	int i, count = 0;
	for (i = blockid + 1; i < line->blockNum; i++) count += 1 + line->block[i].length;
	count--; //because we considered 1 space too many
	
	if (count < (length - 1) - index) {	//means we can't fit all the blocks and spaces
		return 0;
	}
	return 1;
}



/** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
  * solveline:				Solves cells based on the line's native characteristics, ie block number 	*
  * O(TODO)					and block sizes, COUPLED WITH all information that's already been gathered	*
  *							regarding identified cells.													*
  *																										*
  * @param Puzzle* :		puzzle we're working on														*
  * @param Line* :			line to solve																*
  * @param Stack* :			stack to push perpendicular lines to										*
  * @param int :			ROW|COL - coordinate of this line											*
  *	@return int :			number of solved cells, or -1 if an impossibility was discovered.			*
  * 																									*
  * @verbose : TODO																						*
  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#define NextBlock n++;unknCount=0;fullCount=0;
#define IMPOSSIBLE	-1
int solveline(Puzzle* puzzle, Line* line, Stack* stack, int x) {
	int y = (x == ROW ? COL : ROW);
	int n = 0;
	int solvedCells = 0;
	int unknCount = 0;
	int fullCount = 0;
	int length = puzzle->length[x];
	int i;
	
//	ExportSolution(puzzle, stdout);
	
	for (i = 0; i < puzzle->length[x]; i++) {	//we're going to go through every cell in the line
		if (line->cells[i]->state == STATE_FULL) { //== '#' ?
			fullCount++;
			
			if (unknCount == 0) {	//didnt find any '?'s before this '#'
				/* * * * * * * * * * * * * * * * * * *
				 * Fills out the rest of the block.	 *
		 		 * * * * * * * * * * * * * * * * * * */
		 		for (; i < line->block[n].length; i++) {
					if (line->cells[i]->state == STATE_BLNK) {
						return IMPOSSIBLE;
					} else
					if (line->cells[i]->state == STATE_UNKN) {
						line->cells[i]->state = STATE_FULL;
						solvedCells++;
					}
					fullCount++;
				}
				

				/* * * * * * * * * * * * * * * * * * *
				 * Appends a '-' at the end.		 *
		 		 * * * * * * * * * * * * * * * * * * */
				if (line->cells[i]->state == STATE_FULL) {	//block must be affixed by a '-', if it's '#' then the block is too long
					return IMPOSSIBLE;
				} else
				if (line->cells[i]->state == STATE_UNKN) {
					line->cells[i]->state = STATE_BLNK;
					solvedCells++;
				}

				NextBlock
				continue;

			} else {	//If we did find ?'s before, the filling of #'s will be handled when we get to a '?' after the '#'s
						//Because of that, all we need to do here is check for implied blanks
				
				
				//regardless of which block it is, fill blanks between it and the previous block
				
				//find out how many we've identified
				//TODO
				

				if (n == line->blockNum - 1) {	//if it's the last block, also check what the farthest index it can reach is, fill blanks after that
					int j;
					for (j = i ; j - i < line->block[n].length; j++) {		//skip cells that might belong to the block
						if (j == puzzle->length[x]) break;					//reached the end of the line, get out
						if (line->cells[j]->state == STATE_BLNK) break;		//blank means an early break
					}
					for (; j < puzzle->length[x]; j++) {
						line->cells[j]->state = STATE_BLNK;
						solvedCells++;
					}
				}
			
				continue;		//end of the line, dont bother going anywhere else
			}		
		} else
		if (line->cells[i]->state == STATE_BLNK) {	//== '-' ?
			if (unknCount == 0) { //passed cells == 0?
				continue; //do nothing, just move on
			} else {
				if (unknCount < line->block[n].length) {	//passed cells < block size?
					if (checkA(line, n, unknCount, i, puzzle->length[x])) {	//O(N)
						int j;
						for (j = i; line->cells[j]->state == STATE_UNKN; j++) {	//set previous unkn cells to blank before continuing
							line->cells[j]->state = STATE_BLNK;
							solvedCells++;
						}
						continue;	//goto next cell
					} else {
						return IMPOSSIBLE;	//impossible
					}
				} else {
					if (checkA(line, n, unknCount, i, puzzle->length[x])) {	//O(N)
						//not sure yet, complex shit
						//TODO
						//
					} else {
						if (checkAtilde(line, n, unknCount, i, puzzle->length[x])) {	//O(N)
							//check difference between passed cells and block length,
							//stack the block and fill a few #'s
							//NOTE: if passed cells >> blocksize, we probably have several blocks in this
							//space, need to do check to see which blocks can/must fit in this space, which
							//can/must fit afterward, and stack them
							//do this with recursive calls of checkA with an increasing "n" increment, after mixing checkA and checkAtilde into one
							//TODO
							NextBlock
							continue;
						} else {
							return IMPOSSIBLE;	//impossible
						}
					}
				}
			}
		} else {	//== '?'
			if (fullCount == 0) {	//pre-block unkn's
				unknCount++;
			} else {	//post-block unkn's
				//TODO	check if .. if WHAT?!
				int regStartPos;
				int unknCountPost = 0;
				int j;
				int limit = length - getMinSumOfBlocksAndBlanks(line, n) + 1;	//+1 for the space after the current block
				if (limit < i || limit > length) return IMPOSSIBLE;	//the remaining blocks can't fit or n is out of bounds
				limit = (limit < i + line->block[n].length) ? limit : i + line->block[n].length;	//stop at whichever limit is closer

				/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
				 * Counts the cells that can belong to the block that come after it. *
				 * Based on the cells that come before and the ones that come after, *
				 * identifies which ones MUST belong to the block.					 *
				 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
				for (j = i; j < limit; j++) {	//read from i to limit (non-inclusive)
					if (line->cells[j]->state == STATE_BLNK) break;	//blank found, block can't go past it - also counts as a limit, so break

					else if (line->cells[j]->state == STATE_UNKN) unknCountPost++;	//one more unknown cell post-block

					else {	//we found a new '#'! this means there's a gap of unknows in between 2 groups of '#'
						if (unknCount + fullCount + unknCountPost + 1 <= line->block[n].length) {	//means the cells in the middle must be filled
							
							//find out if the first cluster can belong to the previous block:
							
							
							
							for (; unknCountPost > 0; unknCountPost--) {	//go back and fill with #'s
								line->cells[j-unknCountPost]->state = STATE_FULL;
								fullCount++;
								solvedCells++;
							}
							fullCount++;
						
						
							if (fullCount == line->block[n].length) {	//did the previous step fill the whole block?
								j++;
								if 		(line->cells[j]->state == STATE_FULL) return IMPOSSIBLE;	//block was supposed to have ended
								else if	(line->cells[j]->state == STATE_UNKN) {
									line->cells[j]->state = STATE_BLNK;
									solvedCells++;
								}
							}
							
							continue;		//and then continue where we left off, but with the block now fuller
						
						} else {	//means the cells in the middle might not be filled
							
							//TODO
							
						}
					}
				}
				i = j;
				//TODO - this is after all cells that can belong to the block have been identified, so now we can find out which ones MUST belong to it
				
				i-= 1 + unknCountPost;	//so we start counting the unkns into unknCount for the next block
				NextBlock
				continue;
			}
			
			//careful if passed cells >> block length TODO
			continue;
		}
	}
	return solvedCells;
}
#undef IMPOSSIBLE
#undef NextBlock

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
  * getMinSumOfBlocksAndBlanks:	O(N)	Adds up the sizes of all blocks, and then adds (blocknum - 1)	*
  * 									for the mandatory amount of blanks.								*
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
	return count + (line->blockNum - n - 1);	//mandatory spaces
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
  * =   O(N²)										, dN²/dL >> d(3*L)/dL					*
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

	ExportSolution(puzzle, stdout);

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
			unsolvedCellCount -= solveline(puzzle, Pop(stack[ROW]), stack[COL], ROW);
		}
		if (!IsStackEmpty(stack[COL])) {
			unsolvedCellCount -= solveline(puzzle, Pop(stack[COL]), stack[ROW], COL);
		}
//		unsolvedCellCount -= a;
	}
	
	ExportSolution(puzzle, stdout);
	exit(0);
	
	if (unsolvedCellCount > 0) {
		int row = 0, col = 0;
		Cell* pick = PickCell(puzzle);	//O(L²) TODO but might be improved
		Stack* cellstack = CreateStack();
		
		pick->state = STATE_FULL;
		Push(stack[ROW], (void*) &puzzle->line[ROW][row]);
		Push(stack[COL], (void*) &puzzle->line[COL][col]);
//		metasolve(puzzle, stack, unsolvedCellCount - 1, cellstack);
		while (!IsStackEmpty(cellstack)) {
			((Cell*) Pop(cellstack))->state = STATE_UNKN;
		}
		
		pick->state = STATE_BLNK;
		Push(stack[ROW], (void*) &puzzle->line[ROW][row]);
		Push(stack[COL], (void*) &puzzle->line[COL][col]);
//		metasolve(puzzle, stack, unsolvedCellCount - 1, cellstack);
/*		while (!IsStackEmpty(cellstack)) {
			((Cell*) Pop(cellstack))->state = STATE_UNKN;
		}
		
		pick->state = STATE_UNKN;*/
		
		ClearStack(cellstack);
		free(cellstack);
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
int presolve(Puzzle* puzzle) {	//O((Lr+Lc)*N²) = O(2*L*N²) = O(L*N²)
	int i, unsolvedCellCount = puzzle->length[ROW] * puzzle->length[COL];
	//O(Lr*N²)
	for (i = 0; i < puzzle->length[ROW]; i++) unsolvedCellCount -= stackline(&puzzle->line[ROW][i], puzzle->length[ROW]);	
	//O(Lc*N²)
	for (i = 0; i < puzzle->length[COL]; i++) unsolvedCellCount -= stackline(&puzzle->line[COL][i], puzzle->length[COL]);	
	
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
	free(stack);
}



//O(L²) + O(2L) + O(L*N²) + O(TODO) + O(L²) + O(1) + O(1) =
//O(2L² + 2L + 2 + L*N² + TODO)

int main (int n, char** args) {
	if (n < 2) errorout(ERROR_ARGS, "No file name was given.");
	
	Puzzle* puzzle = getPuzzle(args[1]);	//O(L²)
	int i;
	
	Stack** stack = (Stack**) malloc(AXES*sizeof(Stack*));	
	stack[ROW] = CreateStack();
	for (i = 0; i < puzzle->length[ROW]; i++)	Push(stack[ROW], (void*) &puzzle->line[ROW][i]);	//O(Lr)
	stack[COL] = CreateStack();
	for (i = 0; i < puzzle->length[COL]; i++)	Push(stack[COL], (void*) &puzzle->line[COL][i]);	//O(Lc)

	int unsolvedCellCount;
	if ((unsolvedCellCount = presolve(puzzle)) != 0)	//O(L*N²)
		solve(puzzle, stack, unsolvedCellCount);		//O(TODO)

	FreePuzzle(puzzle);	//O(L²)
	FreeStacks(stack);	//O(1)
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
