#ifndef _INCLUDE_STACKS
#define _INCLUDE_STACKS

typedef void Item;

typedef struct _element {
	Item* item;
	struct _element* next;
} Element;

typedef struct _stack {
	Element* top;
} Stack;

Stack*	CreateStack();
void	Push(Stack*, Item*);
Item*	Pop(Stack*);
void	ClearStack(Stack*);
int		IsStackEmpty(Stack*);
int		IsInStack(Stack*, Item*);

#endif
