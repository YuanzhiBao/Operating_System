#include <xinu.h>
#include <future.h>

syscall future_set(future *f, int *value) {
  intmask mask;

  mask = disable();
  if (f == NULL) {
    restore(mask);
    return SYSERR;
  }

  if (f->state == FUTURE_VALID) {  
    restore(mask);
    return SYSERR;
  }

  *f->value = *value;
  if (f->state == FUTURE_EMPTY) {
    f->state == FUTURE_VALID;
  } else if (f->state == FUTURE_WAITING) {
    ready(f->pid); // Switching
  } else {
    restore(mask);
    return SYSERR;
  }
  restore(mask);
  return OK;
}

