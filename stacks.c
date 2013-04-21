Stack* CreateStack() {
	Stack* stack = (Stack*) malloc(sizeof(Stack));
	stack->top = NULL;
}

void Push(Stack* stack, Item* item) {
	Element* element = (Element*) malloc(sizeof(Element));
	Element* buffer = stack->top;
	stack->top = element;
	stack->top->next = buffer;
	stack->top->item = item;
}

Item* Pop(Stack* stack) {
	Element* top = stack->top;
	Item* item = top->item;
	stack->top = stack->top->next;
	free(top);
}
