#include <stdlib.h>
#include "LinkedList.h"

int int_compare(void* a, void* b) {
	int* x = a;
	int* y = b;

	if (*x > *y) return 1;
	else if (*x < *y) return -1;
	return 0;
}

int main() {

}

//int main() {
//		
//
//	return 0;
//}