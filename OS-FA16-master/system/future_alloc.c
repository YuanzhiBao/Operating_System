#include <xinu.h>
#include <future.h>

future* future_alloc(int future_flag) {
  if (future_flag != FUTURE_EXCLUSIVE) {
    return NULL;
  }
  future *fut = (future *)getmem(sizeof(future));
  fut->flag = future_flag;
  fut->state = FUTURE_EMPTY;
  fut->value = (int *)getmem(sizeof(int));
  return fut;
}
