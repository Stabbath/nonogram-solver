#include <stdlib.h>
#include "stacks.h"

Stack* CreateStack() {
	Stack* stack = (Stack*) malloc(sizeof(Stack));
	stack->top = NULL;
	return stack;
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
	return item;
}

void ClearStack(Stack* stack) {
	while (Pop(stack) != NULL);
}

int IsStackEmpty(Stack* stack) {
	return (stack->top == NULL);
}
