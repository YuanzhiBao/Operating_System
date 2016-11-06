#include <xinu.h>
#include <future.h>

syscall future_get(future *f, int *value) {
  intmask mask; // Saved interrupt mask
  struct procent *prptr; // Ptr to process' table entry
  struct sentry *semptr; // Ptr to sempahore table entry
  
  // enter critical section
  mask = disable();
  if (f == NULL) {
    restore(mask);
    return SYSERR;
  }

  if (f->state == FUTURE_VALID) {
    f->state = FUTURE_EMPTY;
    *value = *f->value;
  } else if (f->state == FUTURE_EMPTY) {
    f->state = FUTURE_WAITING;
    prptr = &proctab[currpid];
    prptr->prstate = PR_WAIT; // Set state to waiting 
    f->pid = currpid; // Save Pid
    resched(); // and reschedule
    f->state = FUTURE_EMPTY;
    *value = *f->value;
  } else {
    restore(mask);
    return SYSERR;
  }

  // exit critical section
  restore(mask);
  return OK;
}

