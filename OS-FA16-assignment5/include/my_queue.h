#ifndef _MY_QUEUE_H_
#define _MY_QUEUE_H_


typedef struct q_node_st queue_node;
struct q_node_st {
  queue_node *prev, *next;
  int32 val;
};


typedef struct queue_st queue;
struct queue_st {
  queue_node head, tail;
};

void queue_init(queue *q);
void queue_enqueue(queue *q, int32 val);
int32 queue_dequeue(queue *q);
bool queue_empty(queue *q);

 
#endif /* _MY_QUEUE_H_ */
