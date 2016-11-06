#include <xinu.h>
#include <my_queue.h>
#include <future.h>

static void future_get_check_queues(future *f) {
  // check queues
  bool e_setq = queue_empty(&f->set_queue);
  bool e_getq = queue_empty(&f->get_queue);
  if (e_setq && e_getq) {
    f->state = FUTURE_EMPTY;
  } else if (e_setq) { // then get_q is not empty
    f->state = FUTURE_WAITING;
  } else if (e_getq) { // then set_q is not empty
    f->state = FUTURE_VALID;
  } else { // both set_queue and get_queue have item
    // ready set, and put getpid in f->pid for later awake this get from
    // future_set
    pid32 getpid = (pid32)queue_dequeue(&f->get_queue);
    pid32 setpid = (pid32)queue_dequeue(&f->set_queue);
    f->pid = getpid;
    resume(setpid);
  }
}
// enqueue future_get and modify f->state
// suspend currpid
void future_get_goto_wait(future *f) {
  //enqueue this get
  queue_enqueue(&f->get_queue, currpid);
  // set f state
  f->state = FUTURE_WAITING;
  suspend(currpid);
}

// dequeue future_set and save selfpid to f->pid
// suspend currpid
static void future_get_go_dequeue_set(future *f) {
  pid32 setpid = (pid32)queue_dequeue(&f->set_queue);
  struct procent *prptr; // Ptr to process' table entry 
  prptr = &proctab[currpid];
  prptr->prstate = PR_SUSP; // Set state to waiting
  f->pid = currpid;
  resume(setpid);
}

static void future_get_wait_getval_chkq(future *f, int *value) {
  future_get_goto_wait(f);
  // after future_set set value then get the value
  *value = *f->value;
  f->pid = 0;
  // finally check queues and modify f->state
  future_get_check_queues(f);
}

static void future_get_deqset_getval_chkq(future *f, int *value) {
  // dequeue a set and resume it
  future_get_go_dequeue_set(f);
  // after future_set set value then get the value
  *value = *f->value;
  f->pid = 0;
  // finally check queues and modify f->state
  future_get_check_queues(f);
}

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

  if (f->flag == FUTURE_EXCLUSIVE) {
    if (f->state == FUTURE_VALID) {
      f->state = FUTURE_EMPTY;
      *value = *f->value;
    } else if (f->state == FUTURE_EMPTY) {
      f->state = FUTURE_WAITING;
      prptr = &proctab[currpid];
      prptr->prstate = PR_SUSP; // Set state to waiting
      f->pid = currpid; // Save Pid
      resched(); // and reschedule
      f->state = FUTURE_EMPTY;
      *value = *f->value;
    } else {
      restore(mask);
      return SYSERR;
    }
  } else if (f->flag == FUTURE_SHARED) {
    if (f->state == FUTURE_VALID) {
      *value = *f->value;
    } else if (f->state == FUTURE_WAITING || f->state == FUTURE_EMPTY) {
      f->state = FUTURE_WAITING;
      prptr = &proctab[currpid];
      prptr->prstate = PR_SUSP;
      queue_enqueue(&f->get_queue, currpid);
      resched();
      *value = *f->value;
    } else {
      restore(mask);
      return SYSERR;
    }
  } else if (f->flag == FUTURE_QUEUE) {
    if (f->state == FUTURE_WAITING || f->state == FUTURE_EMPTY) {
      future_get_wait_getval_chkq(f, value);
    } else if (f->state == FUTURE_VALID) {
      if (f->pid != 0) {
        future_get_wait_getval_chkq(f, value);
      } else {
        future_get_deqset_getval_chkq(f, value);
      }
    }
  } else {
    restore(mask);
    return SYSERR;
  }
  // exit critical section
  restore(mask);
  return OK;
}

