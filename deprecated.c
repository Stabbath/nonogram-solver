//solver.c

Line* GetLeftmost(Line* old, int length) {
	Line* line = CloneLine(old, length);
	
	int i, j;
	for (i = 0, j = 0; i < line->blockNum; i++) {
		while (j < line->block[i].length)
			line->cells[j++]->state = STATE_FULL;
		if (j < length)
			line->cells[j++]->state = STATE_BLNK;
	}
	while (j < length)
		line->cells[j++]->state = STATE_UNKN;
	
	return line;
}

Line* GetRightmost(Line* old, int length) {
	Line* line = CloneLine(old, length);

	int i, j = 0;
	int offset = length - getMinSumOfBlocksAndBlanks(line);
	while (j < offset)
		line->cells[j++]->state = STATE_UNKN;

	for (i = 0; i < line->blockNum; i++) {
		while (j - offset < line->block[i].length)
			line->cells[j++]->state = STATE_FULL;
		if (j < length)
			line->cells[j++]->state = STATE_BLNK;
	}
	
	return line;
}

Line* CloneLine(Line* old, int length) {
	Line* new = (Line*) malloc(sizeof(Line));
	int i;
	new->blockNum = old->blockNum;
	new->block = (Block*) malloc(new->blockNum*sizeof(Block));
	for (i = 0; i < new->blockNum; i++) {
		new->block[i] = old->block[i];
	}
	new->cells = (Cell**) malloc(length*sizeof(Cell*));
	for (i = 0; i < length; i++) {
		new->cells[i] = (Cell*) malloc(sizeof(Cell));
		new->cells[i]->state = old->cells[i]->state;
	}
	return new;
}




//solverio.c

void getBlockLengths(Puzzle* puzzle, FILE* fp, int x) {
	int i, j, n, index; 
	char buffer[BUFFERSIZE];
	for (i = 0; i < puzzle->length[x]; i++) {	//next L|C lines have row|column block lengths
		if (fgets(buffer, sizeof(buffer), fp) == NULL)	errorout(ERROR_BADFORMAT, "File ended unexpectedly (block lengths).");

		sscanf(buffer, " %d", &puzzle->line[x][i].blockNum);		//careful with the case of having 0 blocks! the read line will be simply '0'
		if (puzzle->line[x][i].blockNum > 0) {
			if ((puzzle->line[x][i].block = (Block*) malloc(puzzle->line[x][i].blockNum*sizeof(Block))) == NULL)	errorout(ERROR_MEM, "Could not create *block[ROW].");
		} else {	//special case: 0 means 1 block of 0 length (ie no blocks) (negative numbers will be considered 0)
			if ((puzzle->line[x][i].block = (Block*) malloc(sizeof(Block))) == NULL)	errorout(ERROR_MEM, "Could not create *block[ROW].");
			puzzle->line[x][i].block[0].length = 0;
			continue;
		}

		for (n = 0, index = 0; !isspace(buffer[n]); n++) {	//"delete" first number, which determined array size
			buffer[n] = ' ';	//why am i doing this weird stuff with n and index? can't remember
			index = n + 1;
		}

		for (j = 0; j < puzzle->line[x][i].blockNum; j++) {	//read arraySize blocks
			sscanf(buffer, " %d ", &(puzzle->line[x][i].block[j].length));	//get first integer
			for (n = index; !isspace(buffer[n]); n++) {	//replace all characters of the current word (ie the fetched integer) with whitespace
				buffer[n] = ' ';						//the goal of this is to have the "first" integer be the next one, so that with a single 
			}											//sscanf attribution we can read everything
			index = n + 1;	//store starting position of next word (ie next integer) to start on next whitespace-filling loop
		}
	}
}

void copyGridToLines(Puzzle* puzzle, Cell*** cells) {
	int i, j;
	for (i = 0; i < puzzle->length[ROW]; i++) {
		if ((puzzle->line[ROW][i].cells = (Cell**) malloc(puzzle->length[ROW]*sizeof(Cell*))) == NULL)	errorout(ERROR_MEM,"Could not create rows[i].cells.");
		for (j = 0; j < puzzle->length[COL]; j++) {
			puzzle->line[ROW][i].cells[j] = cells[i][j];
		}
	}
	for (i = 0; i < puzzle->length[COL]; i++) {
		if ((puzzle->line[COL][i].cells = (Cell**) malloc(puzzle->length[COL]*sizeof(Cell*))) == NULL)	errorout(ERROR_MEM, "Could not create cols[i].cells.");
		for (j = 0; j < puzzle->length[ROW]; j++) {
			puzzle->line[COL][i].cells[j] = cells[j][i];
		}
	}
}

Cell*** createCellGrid(Puzzle* puzzle) {
	int i, j;
	Cell*** cells;
	if ((cells = (Cell***) malloc(puzzle->length[COL]*sizeof(Cell**))) == NULL)	errorout(ERROR_MEM, "Could not create ***cells.");
	for (i = 0; i < puzzle->length[COL]; i++) {
		if ((cells[i] = (Cell**) malloc(puzzle->length[ROW]*sizeof(Cell*))) == NULL)	errorout(ERROR_MEM, "Could not create **cells.");
		for (j = 0; j < puzzle->length[ROW]; j++) {
			if ((cells[i][j] = (Cell*) malloc(sizeof(Cell))) == NULL)	errorout(ERROR_MEM, "Could not create *cells.");
			cells[i][j]->state = STATE_UNKN;
		}
	}
	return cells;
}

