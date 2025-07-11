#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mem.h"

void *mallocOrDie(size_t size, char *errMess)
{
	void *res = malloc(size);
	if (res==NULL) {
		fprintf(stderr, "%s\n", errMess);
		exit(1);
	}
	memset(res, 0, size);
	return(res);
}
