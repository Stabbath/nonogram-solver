#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "stacks.h"
#include "solver.h"

/** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
  * presolve: O(L*N²)		Calls stackline on every row and on every column once to solve	 			*
  *							the easiest cells, to pave the way for the fuller linesolver.				*
  *																										*
  * @param Puzzle* :		puzzle to preprocess														*
  *	@return int :			number of unsolved cells left in the puzzle									*
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



#define IMPOSSIBLE	-1
/** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
  * stackline: O(N²)		Solves cells implicitly based on the line's native 							*
  *							characteristics, ie block number and block sizes.							*
  *																										*
  * NOTE: this routine will never provide new information if used more than once for a single line.		*
  *																										*
  * @param Line* :			line to solve																*
  * @param int :			length of this line 														*
  *	@return int :			number of solved cells, or -1 if an impossibility was discovered.			*
  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int stackline(Line* line, int length) {
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
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
		 *  Uses simple math to can identify implied cells for general cases *
		 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
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
