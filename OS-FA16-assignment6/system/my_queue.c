#include <xinu.h>
#include <my_queue.h>

//diaozhatian
void queue_init(queue *q) {
  q->head.next = &q->tail;
  q->head.prev = NULL;
  q->tail.next = NULL;
  q->tail.prev = &q->head;
}

void queue_enqueue(queue *q, int32 val) {
  queue_node *node = (queue_node *)getmem(sizeof(queue_node));
  node->val = val;
  node->next = q->head.next;
  node->prev = &q->head;
  q->head.next->prev = node;
  q->head.next = node;
  return;
}
int32 queue_dequeue(queue *q) {
  queue_node *node = q->head.next;
  if (node == &q->tail) {
    return -1;
  }
  q->head.next = node->next;
  node->next->prev = &q->head;
  int32 val = node->val;
  freemem(node, sizeof(queue_node));
  return val;
}

bool queue_empty(queue *q) {
  return q->head.next == &q->tail;
}


