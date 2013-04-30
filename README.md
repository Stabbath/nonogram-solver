#nonogram-solver!

Title is self-explanatory: code for a nonogram-solving program. Only for single-colored puzzles.

##How it works!
###Overview
The puzzle is broken up into a series of lines, 1 for each row and 1 for each column, which are all handled separately. The reasoning behind this is that the solution for a line is not directly dependent of any others: simply of the number and length of its blocks and of the information already gathered about it. Since each row and column pair have 1 cell in common, this means that when you discover information on one line, you have also discovered information on other lines. Then when you solve those lines, you once again find information that is shared with other lines, and so on, recursively, until the puzzle is solved.

In extreme situations, especially in cases where the puzzle is complex AND has multiple solutions, or even a few rare cases where it has no solutions but the impossibility is not obvious, it might be necessary to 'guess' solutions. In these situations, the program will select an unidentified cell, and test solving the puzzle again after forcing it to both possible values. This is done recursively, so even if the first selection isn't helpful, the program will keep running until all possible solutions (if there are any) are found.

Currently, the algorithm is extremely incomplete: 'solving a line' simply means overlapping the leftmost and rightmost possible solutions of the line. The tracking of impossibilities is hardly done either! Still a lot of work to do.

###Line solver
The following is a text representation/pseudocode of the current state of the algorithm being considered for a general line solver.

NextBlock: block = next, passed cells = 0	//next = first on the first time
A: read cell	//next = first on the first time
is it #?
	NO: is it -?
		NO: passed cells++
			goto A
		YES: goto A
	YES: passed cells == 0?
		NO: goto Uncentered
		YES: goto Easy
//
Easy: read cell
is it #?
	YES: goto Easy
	NO: is it -?
		YES: cell index == block length?
			NO: goto Impossible
			YES: goto NextBlock
Check:	NO: cell index == block length?
			YES: read cell
				is it #?
					YES: goto Impossible
					NO: set to -
						goto NextBlock
			NO: read cell
				is it -?
					YES: goto Impossible
					NO: set to #
						goto Check
//
Uncentered:	//working on it


//
Impossible:
get out, no solution for this case


###Brute force
Many puzzles may be impossible to solve by the regular line-solving algorithm, at which point we must resort to a brute force mechanism: we find a cell that hasn't been determined, and we test if for both values.
At first this was done by fully cloning the puzzle, but this constitutes a huge memory expense, and there is a much better way that saves a lot of memory while barely adding any computing time: storing the pointers of this changed cell and any cell changed as a consequence in a list, and upon returning from the recursive call to solve the puzzle with this new guessed information, resetting the values of all those cells to unknown.

###Stacks
####Main
The source code defines a stack structure, along with an interface for interacting with said structure. The need for this arose out of a simple concept: the more information we have on a line, the more information we're likely to get.

Imagine we are solving a row and discover that it's 4th cell must be FULL. This cell that we've identified is also part of a perpendicular column: the 4th column to be precise. This means we have uncovered new information on the 4th column, and thus if we re-examine it, we are likely to be able to uncover even more information! A graphic example follows.

Let's take a simple premise: say that the program starts by solving the 4th column, followed by the 1st, and then it starts solving lines from the stack, and as it identifies each cell it pushes its perpendicular line onto the stack of lines that should be solved.

x0 1 2 0  
0? ? ? ?  
0? ? ? ?  
1? ? ? ?  
2? ? ? ?  

We start solving the 4th column: the solution to it is instantaneous.

x0 1 2 0  
0? ? ? ␣  
0? ? ? ␣  
1? ? ? ␣  
2? ? ? ␣  

So the perpendicular rows were added to the stack, the 4th one being last. Before using the stack, the program still solves the 1st column, by hypothesis:

x0 1 2 0  
0␣ ? ? ␣  
0␣ ? ? ␣  
1␣ ? ? ␣  
2␣ ? ? ␣  

Once again the rows are added to the stack, the 4th one being last once more. Now the program uses the stack: we solve the 4th row.

x0 1 2 0  
0␣ ? ? ␣  
0␣ ? ? ␣  
1␣ ? ? ␣  
2␣ # # ␣  

The fact that we had identified 2 cells in this row made it so that we could identify the remaining 2. This is a loose example, but it's basically common sense that what is being proposed is true.

The way it currently works is that we treat rows and columns differently, so that we can keep track of which ones are rows and which ones are columns, because as it is right now the length of rows and columns is defined in the main structure, Puzzle, rather than inside of each line. This is because all rows have the same length, and all columns have the same length, so it hardly warrants the additional memory usage. This might be changed in the future, since the alternative would simplify code.

####Did it have to be a stack?
No, in fact the first thing that occured to me was a FIFO list, but after manually solving a few puzzles following the described logic, it appeared that stacks were always better than FIFO. This most likely (read: definitely) varies from puzzle to puzzle, but stacks have the added benefit of being a tiny bit simpler (only need to track the top). Not much of an argument, but the kind of list is not really significant, as long as it's simple and straightforward in terms of the addition and removal of elements.

####Possible developments
Might use a single stack for rows and columns in the future, as described at the end of the main stacks section.  
Will use a different structure if it turns out that there's something much more efficient than stacks for a majority of puzzles. (doubtful)  

###todo:
####Priorities
Finish functions to free allocated memory.  
Test with valgrind to make sure everything's good in that department.  
Improve and implement line solving algorithm.  
####Secondary
Write function descriptions and add missing functions to headers.  
Continue writing readme, including mentions of previous 2d Cell array scheme and whatnot, maybe mention debug functions.  
Make sure all memory allocs will errorout if return pointer is null, and other similar things.  
Change ExportSolution to export solutions into (a) text file(s) rather than print to console.  
