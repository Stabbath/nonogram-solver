/*
Cenas em que eu pensei antes de começar a escrever o código e isso:

//maybe track number of unsolved cells?
//talvez sobrepor a linha/coluna perpendicular cada vez que um quadrado novo é identificado?
//guardar posicoes de começo e fim de cada bloco (não só os X blocos da linha, mas todos os blocos identificáveis ao longo da procura da solução. por exemplo um bloco de 3 de que é desconhecido o quadrado do meio pode ser considerado 2 blocos)

resolve_linha:
	M = valor para o comprimento do bloco mais longo da linha
	S = soma dos comprimentos de todos os blocos + o numero de espaços mínimo (= numero de blocos - 1)
	
	se (largura do puzzle - S < M)	//nota: se largura == S, só há uma solução para a linha
		cria 2 linhas fora do puzzle, ambas cópias da original
		faz delas as soluções extremas para a esquerda e direita
		sobrepoe

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


forca_solucoes é aquela ideia para quando o algoritmo chega a um ponto em que nao consegue avançar mais: adivinha um valor para algum bloco (que ou é escolhido aleatoriamente ou então é escolhido de uma forma inteligente com base nos quadrados que já se conhecem), e continua todo o processo com isto. cada vez que volta a ficar encravado, adivinha uma nova posiçao. quando chega ao fim de uma tentativa, vê se a solução é válida ou inválida. se a solução for impossível volta um nível de recursão atrás e tenta o valor oposto no mesmo quadrado. se for de novo inválida, volta para trás para o nível de recursão em que foram tentadas as 2 possibilidades. e volta mais um para atrás, porque agora sabemos que o valor dado nesse é impossível. por isso tentamos o oposto. etc.

//se a solução for válida, será preciso voltar atrás à mesma, já que apenas descobrimos uma solução: podem haver mais

provavelmente vão ser preciso mais estruturas dentro da estrutura puzzle, para guardar informação em relação a cada bloco - onde começa, onde acaba, se está completamente descoberto, possivelmente mais coisas
*/

//remember this:
//sobrepoe_linhas para cada linha e cada coluna uma vez
//guardar o numero da coluna/linha perpendicular à linha/coluna em que cada ponto é descoberto numa lista FIFO, não repetir números
//sobrepoe_linhas para cada linha/coluna da lista, continuando a guardar os pontos na lista FIFO, continuando sem repetir números
//repetir até a lista estar vazia ou o puzzle estar resolvido

//se o puzzle não estiver resolvido:
//fazer backup do puzzle
//meter um valor aleatório (ou inteligentemente escolhido) num quadrado aleatório, resolver o puzzle com base nisso, repetir o bloco anterior
//se a lista ficar vazia outra vez e o puzzle não estiver resolvido, fazer backup mais uma vez, repetir (recursão)

//quando o puzzle estiver resolvido, se for descoberto que a solução está errada, volta-se um nível de recursão atrás, e tenta-se para o valor oposto no mesmo quadrado. se a solução estiver errada outra vez, volta-se atrás um nível, e depois volta-se atrás outra vez, e tenta-se para o valor oposto, etc. ad infinitum

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "stacks.h"

#define ERROR_BADFORMAT "Error! Invalid file formatting."

#define BUFFERSIZE	64

#define STATE_FULL '#'	//states are characters so that they can be printed directly
#define STATE_BLNK '-'	//
#define STATE_UNKN '?'	//

//all reused data is stored by pointers: each cell is referenced by the same pointer whether it's accessed from 'cells' or 'rows'/'cols'

typedef struct _cell {
	char state;		//STATE_FULL, STATE_BLNK, STATE_UNKN
} Cell;

typedef struct _line {
	Cell** cells;	//array of pointers to cells
} Line;

typedef struct _puzzle {
	int unsolvedCellCount;		//number of cells left unsolved
	int rowNum, colNum;			//number of rows and columns
	int** rowBlks, ** colBlks;	//2d array of integers for block lengths
	Line* rows, * cols;			//arrays of lines
	Cell*** cells;				//2d array of cell pointers					//is there actually a point in having this if lines are parsed via a different variable
} Puzzle;

void errorout(char* type, char* details) {
	printf("%s %s\n", type, details);
	exit(1);
}

void checkblankln(FILE* fp) {
	char buffer[BUFFERSIZE];
	if (fgets(buffer, sizeof(buffer), fp) == NULL) {	//do we even have a line to read?
		errorout(ERROR_BADFORMAT, "File ended unexpectedly (blank line).");
	}
	if (strlen(buffer) != 0) {	//line is there but not blank!
		errorout(ERROR_BADFORMAT, "Blank line was not blank.");
	}
}


Puzzle* getPuzzle(char* name) {
	FILE* fp = fopen(name, "r");

	if (fp == NULL) {
		printf("error");
		exit(1);
	}
	
	Puzzle* puzzle = (Puzzle*) malloc(sizeof(Puzzle));
	
	char buffer[BUFFERSIZE];
	
	if (fgets(buffer, sizeof(buffer), fp) == NULL) {	//get 1st line - L = number of rows
		errorout(ERROR_BADFORMAT, "File ended unexpectedly (row num).");
	}
	if (sscanf(buffer, "%d", &(puzzle->rowNum)) <= 0) {
		errorout(ERROR_BADFORMAT, "Row number not an integer.");
	}
	if (fgets(buffer, sizeof(buffer), fp) == NULL) {	//get 2nd line - C = number of columns
		errorout(ERROR_BADFORMAT, "File ended unexpectedly (column num).");
	}
	if (sscanf(buffer, "%d", &(puzzle->colNum)) <= 0) {
		errorout(ERROR_BADFORMAT, "Column number not an integer.");	
	}
	checkblankln(fp);	//3rd line - must be blank
	
	puzzle->rowBlks = (int**) malloc(puzzle->rowNum*sizeof(int*));
	puzzle->colBlks = (int**) malloc(puzzle->colNum*sizeof(int*));

	int i, j;
	puzzle->cells = (Cell***) malloc(puzzle->colNum*sizeof(Cell**));
	for (i = 0; i < puzzle->colNum; i++) {
		puzzle->cells[i] = (Cell**) malloc(puzzle->rowNum*sizeof(Cell*));
		for (j = 0; j < puzzle->rowNum; j++) {
			puzzle->cells[i][j] = (Cell*) malloc(sizeof(Cell));		
			puzzle->cells[i][j]->state = STATE_UNKN;
		}
	}



	for (i = 0; i < puzzle->rowNum; i++) {	//next L lines have row block lengths
		if (fgets(buffer, sizeof(buffer), fp) == NULL) { //get next line
			errorout(ERROR_BADFORMAT, "File ended unexpectedly (row list).");
		}

		int arraySize;
		sscanf(buffer, " %d", &arraySize);		//careful with the case of having 0 blocks! the read line will be simply '0'
		if (arraySize > 0) {
			puzzle->rowBlks[i] = (int*) malloc(arraySize*sizeof(int));
		} else {
			puzzle->rowBlks[i] = (int*) malloc(sizeof(int));
			puzzle->rowBlks[i][0] = 0;
		}

		int n, index = 0;
		for (n = index; !isspace(buffer[n]); n++) {	//"delete" first number, which determined arraySize
			buffer[n] = ' ';
		index = n + 1;
		}

		for (j = 0; j < arraySize; j++) {	//read arraySize blocks
			sscanf(buffer, " %d ", &(puzzle->rowBlks[i][j]));	//get first integer
			for (n = index; !isspace(buffer[n]); n++) {	//replace all characters of the current word (ie the fetched integer) with whitespace
				buffer[n] = ' ';						//the goal of this is to have the "first" integer be the next one, so that with a single 
			}											//sscanf attribution we can read everything
			index = n + 1;	//store starting position of next word (ie next integer) to start on next whitespace-filling loop
		}
	}



	checkblankln(fp);	//get (3+L+1)th line - must be blank


	
	for (i = 0; i < puzzle->colNum; i++) {	//next C columns have col block lengths
		if (fgets(buffer, sizeof(buffer), fp) == NULL) { //get next line
			errorout(ERROR_BADFORMAT, "File ended unexpectedly (column list).");
		}

		int arraySize;
		sscanf(buffer, "%d", &arraySize);		//careful with the case of having 0 blocks! the read line will be simply '0'
		if (arraySize > 0) {
			puzzle->colBlks[i] = (int*) malloc(arraySize*sizeof(int));
		} else {
			puzzle->colBlks[i] = (int*) malloc(sizeof(int));
			puzzle->colBlks[i][0] = 0;
		}

		int n, index = 0;
		for (n = index; !isspace(buffer[n]); n++) {	//"delete" first number, which determined arraySize
			buffer[n] = ' ';
		}
		index = n + 1;

		for (j = 0; j < arraySize; j++) {	//read arraySize blocks
			sscanf(buffer, " %d ", &(puzzle->colBlks[i][j]));	//get first integer
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



	puzzle->unsolvedCellCount = puzzle->rowNum * puzzle->colNum;
	
	puzzle->rows = (Line*) malloc(puzzle->rowNum*sizeof(Line));
	for (i = 0; i < puzzle->rowNum; i++) {
		puzzle->rows[i].cells = (Cell**) malloc(puzzle->rowNum*sizeof(Cell*));
		for (j = 0; j < puzzle->colNum; j++) {
			puzzle->rows[i].cells[j] = puzzle->cells[i][j];
		}
	}
	puzzle->cols = (Line*) malloc(puzzle->colNum*sizeof(Line));
	for (i = 0; i < puzzle->colNum; i++) {
		puzzle->cols[i].cells = (Cell**) malloc(puzzle->colNum*sizeof(Cell*));
		for (j = 0; j < puzzle->rowNum; j++) {
			puzzle->rows[i].cells[j] = puzzle->cells[j][i];
		}
	}

	return puzzle;
}

/**	solve - finds every possible solution to 
 *	@param Puzzle* :	pointer to puzzle that needs solving
 *	@param Stack* : 	stack of rows of interest to solve
 *	@param Stack* : 	stack of columns of interest to solve
 *	@param int :		level of recursion (increases by 1 for each function call before it)
 *	@noreturn
 *
 *	@verbose : uses solveline for every row in the stacks until they are empty or the puzzle has been solved. If the stacks are emptied before the solution is found, selects an unknown cell and tests both possible values. Does all this recursively until all solutions have been found.
 *//*
void solve(Puzzle* p, Stack* stackRow, Stack* stackCol, int n) {
	while (stackRow->first != NULL && stackCol->first != NULL && p.unsolvedCellCount > 0) {
		p.unsolvedCellCount -= solveline(p, stackRow);
		p.unsolvedCellCount -= solveline(p, stackCol);
	}
	
	if (p.unsolvedCellCount > 0) {
		//get a random unknown cell
		
		//set it to STATE_FULL		
		Push(stackRow, puzzle.row[y]);
		Push(stackCol, puzzle.col[x]);
		solve(p, stackRow, stackCol, n + 1);
		
		//set it to STATE_FULL
		Push(stackRow, puzzle.row[y]);
		Push(stackCol, puzzle.col[x]);
		solve(p, stackRow, stackCol, n + 1);
	} else
		//export solution
	}
	
	return;
}*/

int main (int n, char** args) {
	if (n < 2) {
		printf("lol no\n");
		return -1;
	}
		
	Puzzle* puzzle = getPuzzle(args[1]);
	
//	Stack* stackRow = CreateStack();
//	Stack* stackCol = CreateStack();
	
//	int i;
//	for (i = 0; i < puzzle.rowNum; i++)	Push(stackRow, puzzle.rows[i]);
//	for (i = 0; i < puzzle.colNum; i++)	Push(stackRow, puzzle.cols[i]);
	
//	solve(puzzle, stackRow, stackCol, 0);
	
	return 0;	
}
