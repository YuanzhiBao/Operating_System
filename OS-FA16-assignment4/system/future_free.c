#include <xinu.h>
#include <future.h>

syscall future_free(future *fut) {
  freemem(fut->value, sizeof(int));
  return freemem(fut, sizeof(future));
}
