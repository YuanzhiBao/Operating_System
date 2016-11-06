#include <xinu.h>
#include <stdio.h>
#include <stdlib.h>
#include <prodcons.h>
#include <future.h>

int n; //Definition for global variable 'n'
/*Now global variable n will be on Heap so it is accessible all the processes i.e. consume and produce*/

sid32 produced, consumed; // definition of global variables produced and consumed


shellcmd xsh_prodcons(int nargs, char *args[]) {
  n = 0;
  int count;
  
  int use_future = 0;
  
  //Argument verifications and validations
  if (nargs == 1) {
    count = 2000;
  } else if (nargs == 2) {
    if (strcmp (args[1], "-f") == 0) {
      use_future = 1;
    } else if ((count = atoi(args[1])) > 0) {
      ;
    } else {
      printf("Error: wrong option!\n");
      return -1;
    }
  } else {
    printf("Error: wrong options!\n");
    return -1;
  }


  if (use_future == 1) {// prodcons using future
    future *f_exclusive, *f_shared, *f_queue;

    f_exclusive = future_alloc(FUTURE_EXCLUSIVE);
    f_shared = future_alloc(FUTURE_SHARED);
    f_queue = future_alloc(FUTURE_QUEUE);

    // Test FUTURE_EXCLUSIVE
    resume( create(future_cons, 1024, 20, "fcons1", 1, f_exclusive) );
    resume( create(future_prod, 1024, 20, "fprod1", 1, f_exclusive) );

    // Test FUTURE_SHARED
    resume( create(future_cons, 1024, 20, "fcons2", 1, f_shared) );
    resume( create(future_cons, 1024, 20, "fcons3", 1, f_shared) );
    resume( create(future_cons, 1024, 20, "fcons4", 1, f_shared) ); 
    resume( create(future_cons, 1024, 20, "fcons5", 1, f_shared) );
    resume( create(future_prod, 1024, 20, "fprod2", 1, f_shared) );

    // Test FUTURE_QUEUE
    resume( create(future_cons, 1024, 20, "fcons6", 1, f_queue) );
    resume( create(future_cons, 1024, 20, "fcons7", 1, f_queue) );
    resume( create(future_cons, 1024, 20, "fcons7", 1, f_queue) );
    resume( create(future_cons, 1024, 20, "fcons7", 1, f_queue) );
    resume( create(future_prod, 1024, 20, "fprod3", 1, f_queue) );
    resume( create(future_prod, 1024, 20, "fprod4", 1, f_queue) );
    resume( create(future_prod, 1024, 20, "fprod5", 1, f_queue) );
    resume( create(future_prod, 1024, 20, "fprod6", 1, f_queue) );

    // NEED TO FREE f2, f2, f3

  } else { // prodconds using semaphore
    produced = semcreate(0);
    consumed = semcreate(1);

    resume( create(producer, 1024, 20, "producer", 1, count));
    resume( create(consumer, 1024, 20, "consumer", 1, count));  
  }
  return (0);
}
