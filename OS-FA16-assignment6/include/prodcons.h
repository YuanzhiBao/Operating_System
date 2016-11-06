#include <future.h>

/*Global variable for producer consumer*/
extern int n; /*this is just declaration*/

// Use semaphore
void consumer(int count);
void producer(int count);
// Semahore Declaration
extern sid32 produced, consumed;  // delaration of extern variable 

// Use future
uint future_prod(future *fut);
uint future_cons(future *fut);
