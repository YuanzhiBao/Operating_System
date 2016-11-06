#include <xinu.h>
#include <prodcons.h>

void consumer(int count) {
	int i;
	for (i = 0; i < count; i++) {
		printf("this is in consumer and n value is %d\n", n);
	}
}

