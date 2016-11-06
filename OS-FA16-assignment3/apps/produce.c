#include <xinu.h>
#include <prodcons.h>

void producer(int count) {
	int i;
	for (i = 0; i < count; i++) {
//		wait(consumed);
		n++;
		printf("this is in producer and n value is %d\n", n);
		signal(produced);
	}	
}



