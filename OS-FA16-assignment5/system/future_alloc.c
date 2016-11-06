#include <xinu.h>
#include <my_queue.h>
#include <future.h>

future* future_alloc(int future_flag) {
  if (future_flag != FUTURE_EXCLUSIVE && future_flag != FUTURE_SHARED &&
      future_flag != FUTURE_QUEUE) {
    return NULL;
  }
  future *fut = (future *)getmem(sizeof(future));
  fut->flag = future_flag;
  fut->state = FUTURE_EMPTY;
  fut->value = (int *)getmem(sizeof(int));
  fut->pid = 0;
  queue_init(&fut->get_queue);
  queue_init(&fut->set_queue);
  return fut;
}
