#include <stdio.h>
#include <string.h>
#include "mm.h"

int main()
{
	mm_init();
	char *str = "hello world!";
	char *p = mm_malloc(strlen(str) + 1);
	//void *p = NULL;
	if (p == NULL) {
		mm_dump();
		printf("mm_malloc failed\n");
	}
	else {
		strcpy(p, str);
		mm_dump();
		mm_free(p);
		mm_dump();
	}

	mm_exit();
	return 0;
}
