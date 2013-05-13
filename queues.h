#ifndef _INCLUDE_QUEUES
#define _INCLUDE_QUEUES

typedef void Item;
typedef struct _element Element;
typedef struct _queue Queue;

Queue*	CreateQueue();
void	PushQ(Queue*, Item*);
Item*	PopQ(Queue*);
void	ClearQueue(Queue*);
int		IsQueueEmpty(Queue*);
int		IsInQueue(Queue*, Item*);

#endif
