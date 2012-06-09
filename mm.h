#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#define HEAP_SIZE 1024 * 1024
#define WSIZE 4
#define DSIZE 8

#define PACK(size, alloc) ((size) | (alloc))

#define PUT(p, val) 	(*(size_t*)(p) = (val))
#define GET(p)		(*(size_t*)(p))

#define GET_SIZE(p)	(GET(p) & 0xFFFFFFF8)
#define GET_ALLOC(p)	(GET(p) & 0x1)	

/* head and tail of the block*/
#define HEAD(p)		((void*)(p) - WSIZE)
#define TAIL(p)		((void*)(p) + GET_SIZE(HEAD(p)) - DSIZE)

/*previous and next of the block*/
#define NEXT(p)		((void*)(p) + GET_SIZE(HEAD(p)))
#define PREV(p)		((void*)(p) - GET_SIZE((void*)(p) - DSIZE))

static void *mem_start;
static void *mem_end;
static void *mem_brkp;
static void *heap = NULL; /*the head of the heap*/

int mm_init();
void* mm_malloc(size_t);
void mm_free(void*);
void mm_exit();
void mm_dump();

/*
 * malloc the size
 */
static void mem_init()
{
	mem_start	= (void*)malloc(HEAP_SIZE);
	mem_brkp		= mem_start;
	mem_end		= mem_start + HEAP_SIZE;
}

static void *mem_brk(size_t size)
{
	void *old_brk = mem_brkp;

	if (size < 0 || mem_brkp + size > mem_end) {
		return NULL;
	}

	mem_brkp += size;
	return old_brk;
}

