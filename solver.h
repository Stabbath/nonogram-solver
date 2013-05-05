/*
//fazer alocação de mem para puzzles (leitura e copia) através de linhas e colunas, sem cell***: aloca mem para linhas, e depois faz com que cada coluna N aponte para a celula N de cada linha
//usar calculos para determinar as posicoes preenchidas em vez de criar 2 linhas clone e cenas

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

provavelmente vão ser precisas mais estruturas dentro da estrutura puzzle, para guardar informação em relação a cada bloco - onde começa, onde acaba, se está completamente descoberto, possivelmente mais coisas
*/

//remember this:
//guardar o numero da coluna/linha perpendicular à linha/coluna em que cada ponto é descoberto numa lista FIFO, não repetir números

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
int getMinSumOfBlocksAndBlanks(Line*);
int getLengthOfLargestBlock(Line*);

/** stackline - "stacks" the right-most and left-most positionings of all blocks and compares them
 *	@param Puzzle* :	pointer to puzzle that's being worked on
 *  @param Line* :		pointer to line that needs stacking
 *	@param Stack* :		pointer to stack that 'interesting' perpendicular lines should be pushed to
 *	@noreturn
 *
 *	@verbose
 */
//int stackline(Puzzle*, Line*, Stack*, int);

/** solveline - attempts to solve a line
 *	@param Puzzle* :	pointer to puzzle that's being worked on
 *	@param Line* :		pointer to line that needs solving
 *	@param Stack* :		pointer to perpendicular stack, to push interesting lines
 *	@return 			number of solved cells or -length[ROW]*length[COL] if line cannot be solved
 *
 *	@verbose			-length[ROW]*length[COL] is returned so that unsolvedcellcount is necessarily negative.
 */
int solveline(Puzzle*, Line*, Stack*, int);

/**	solve - finds every possible solution to 
 *	@param Puzzle* :	pointer to puzzle that needs solving
 *	@param Stack* : 	stack of rows of interest to solve
 *	@param Stack* : 	stack of columns of interest to solve
 *	@param int :		number of unkn cells in the current solution-attempt of the puzzle
 *	@noreturn
 *
 *	@verbose : uses solveline for every row in the stacks until they are empty or the puzzle has been solved. If the stacks are emptied before the solution is found, selects an unknown cell and tests both possible values. Does all this recursively until all solutions have been found.
 */
void solve(Puzzle*, Stack**, int);

#endif
