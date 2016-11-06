#include <xinu.h>
#include <my_queue.h>
#include <future.h>

//future_set dequeue get from get_queue
// dequeue future_get and save selfpid to f->pid
static void future_set_dequeue_get(future *f) {
  pid32 getpid = (pid32)queue_dequeue(&f->get_queue);
  f->pid = getpid;
  resume(getpid);
}
// enqueue future_set and modify f->state
// suspend currpid
static void future_set_goto_wait(future *f) {
  //enqueue this get
  queue_enqueue(&f->set_queue, currpid);
  // set f state
  f->state = FUTURE_VALID;
  suspend(currpid);
}
static void future_set_enqset_setvalue_runget(future *f, int *value) {
  //future_set goto wait and set the value
  future_set_goto_wait(f);
  *f->value = *value;
  //resume get process
  resume(f->pid);
}
static void future_set_setvalue_savepid_deqget(future *f, int *value) {
  //set the value and set f->pid
  *f->value = *value;
  future_set_dequeue_get(f);
}


syscall future_set(future *f, int *value) {
  intmask mask;
  struct procent *prptr; // Ptr to process' table entry

  pid32 pid;

  mask = disable();
  if (f == NULL) {
    restore(mask);
    return SYSERR;
  }

  if (f->flag == FUTURE_EXCLUSIVE) {
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
  } else if (f->flag == FUTURE_SHARED) {
    if (f->state == FUTURE_VALID) {
      restore(mask);
      return SYSERR;
    } else if (f->state == FUTURE_EMPTY) {
      *f->value = *value;
      f->state = FUTURE_VALID;
    } else if (f->state == FUTURE_WAITING) {
      *f->value = *value;
      f->state = FUTURE_VALID;
      while (!queue_empty(&f->get_queue)) {
        pid = queue_dequeue(&f->get_queue);
        ready(pid);
      }
    } else {
        restore(mask);
        return SYSERR;
    }
  } else if (f->flag == FUTURE_QUEUE) {
      if (f->state == FUTURE_EMPTY || f->state == FUTURE_VALID) {
        future_set_enqset_setvalue_runget(f, value);
      } else {
        if (f->pid != 0) {
          future_set_enqset_setvalue_runget(f, value);
        } else {
          future_set_setvalue_savepid_deqget(f, value);
        }
      }
  } else {
    restore(mask);
    return SYSERR;
  }

  restore(mask);
  return OK;
}

