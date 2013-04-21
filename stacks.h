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

Stack* CreateStack() {}
void Push(Stack* stack, Item* item) {}
Item* Pop(Stack* stack) {}

#endif
