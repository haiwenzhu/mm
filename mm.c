#include "mm.h"

/*
 * merge the adjacent block
 */
static void *coalesce(void *p)
{
	size_t size = GET_SIZE(HEAD(p));
	size_t prev_size = GET_SIZE(HEAD(PREV(p)));
	size_t next_size = GET_SIZE(HEAD(NEXT(p)));
	int prev_alloc = (int)GET_ALLOC(HEAD(PREV(p)));
	int next_alloc = (int)GET_ALLOC(HEAD(NEXT(p)));

	/*case1: the previous block is free*/
	if (!prev_alloc && next_alloc) {
		PUT(TAIL(p), PACK(size + prev_size, 0));
		PUT(HEAD(PREV(p)), PACK(size + prev_size, 0));
		p = PREV(p);
	}
	/*case2: the next block is free*/
	else if (prev_alloc && !next_alloc) {
		PUT(HEAD(p), PACK(size + next_size, 0));
		PUT(TAIL(NEXT(p)), PACK(size + next_size, 0));
	}
	/*case3: the revious and next block is free*/
	else if (!prev_alloc && !next_alloc) {
		PUT(HEAD(PREV(p)), PACK(size + prev_size + next_size, 0));
		PUT(TAIL(NEXT(p)), PACK(size + prev_size + next_size, 0));
		p = PREV(p);
	}

	return p;
}

/*
 * extend the heap with a block 
 */
static void *extend_heap(size_t size)
{
	void *p;
	if ((p = mem_brk(size)) == NULL) {
		return NULL;
	}

	PUT(HEAD(p), PACK(size, 0));
	PUT(TAIL(p), PACK(size, 0));
	PUT(HEAD(NEXT(p)), PACK(0, 1));

	return coalesce(p);
}

/*
 * find a free block which has a bigger size then size
 */
static void *find_fit(size_t size)
{
	void *p = heap;
	while (GET_SIZE(HEAD(p)) > 0) {
		if (!GET_ALLOC(HEAD(p)) && GET_SIZE(HEAD(p)) > size) {
			break;
		}
		p += GET_SIZE(HEAD(p));
	}

	if (GET_SIZE(HEAD(p)) == 0) {
		p = extend_heap(size);
	}

	return p;
}

/*
 * split the block if the block has more free space
 */
static void place(void *p, size_t size)
{
	size_t remaind_size = GET_SIZE(HEAD(p)) - size;

	if (remaind_size < 2 * DSIZE) {
		PUT(HEAD(p), PACK(size, 1));
		PUT(TAIL(p), PACK(size, 1));
		return;
	}
	
	PUT(HEAD(p), PACK(size, 1));
	PUT(TAIL(p), PACK(size, 1));
	PUT(HEAD(NEXT(p)), PACK(remaind_size, 0));	
	PUT(TAIL(NEXT(p)), PACK(remaind_size, 0));

	return;
}

/*
 * init the heap
 */
int mm_init()
{
	if (heap != NULL) {
		return;
	}

	mem_init();

	if ((heap = mem_brk(4*WSIZE)) == NULL) {
		return -1;
	}

	PUT(heap, 1);
	PUT(heap + WSIZE, PACK(8, 1)); /*the head of the block*/
	PUT(heap + DSIZE, PACK(8, 1));
	PUT(heap + DSIZE + WSIZE, PACK(0, 1));	/* the tail of the block*/

	heap += DSIZE;

	return 0;
}	

void *mm_malloc(size_t size)
{
	void *p;
	size_t asize;

	if (size < 0) {
		return NULL;
	}

	/*memory align*/
	asize = DSIZE * ((size + DSIZE - 1) / DSIZE) + DSIZE;
	if ((p = find_fit(asize)) != NULL) {
		place(p, asize);
	}

	return p;
}

void mm_free(void *p)
{
	if (p == NULL) {
		return;
	}

	size_t size = GET_SIZE(HEAD(p));

	PUT(HEAD(p), PACK(size, 0));
	PUT(TAIL(p), PACK(size, 0));

	coalesce(p);
	return;
}

void mm_exit()
{
	free(mem_start);
	return;
}

/*
 * dump the content of the heap
 */
void mm_dump()
{
	void *p = heap;

	printf("mm_dump begin:\n");
	//p = NEXT(p);
	while (GET_SIZE(HEAD(p)) != 0) {
		size_t size = (size_t)GET_SIZE(HEAD(p));
		int alloc   = (int)GET_ALLOC(HEAD(p));
		if (alloc > 0) {
			printf("{SIZE:%d;ALLOC:%d;CONTENT:%s}\n", size, alloc, p);	
		}
		else {
			printf("{SIZE:%d;ALLOC:%d;CONTENT:UNUSED}\n", size, alloc);	
		}

		p = NEXT(p);
	}
	return;
}
