/*
 * mm-naive.c - The fastest, least memory-efficient malloc package.
 * 
 * In this naive approach, a block is allocated by simply incrementing
 * the brk pointer.  A block is pure payload. There are no headers or
 * footers.  Blocks are never coalesced or reused. Realloc is
 * implemented directly using mm_malloc and mm_free.
 *
 * NOTE TO STUDENTS: Replace this header comment with your own header
 * comment that gives a high level description of your solution.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

/*********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your team information in the following struct.
 ********************************************************/
team_t team = {
    /* Team name */
    "ateam",
    /* First member's full name */
    "Harry Bovik",
    /* First member's email address */
    "bovik@cs.cmu.edu",
    /* Second member's full name (leave blank if none) */
    "",
    /* Second member's email address (leave blank if none) */
    ""
};

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)


#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

//定义基本的宏
#define WSIZE 4
#define DSIZE 8
#define INITCHUNSIZE (1 << 6)
#define CHUNKSIZE (1 << 12)

#define MAX(x, y) ((x) > (y) ? (x) : (y))
#define PACK(size, alloc) ((size) | (alloc)) //将分配块的大小和最后一位进行按位或
#define GET(p) (*(unsigned int *)(p)) //获取一个字的内容
#define PUT(p, val)(*(unsigned int *)(p) = (val)) //为一个字赋值
#define GET_SIZE(p) (GET(p) & ~0x7) //获取header或者footer存储的大小
#define GET_ALLOC(p) (GET(p) & 0x1) //获取header或者footer存储的分配位
#define HDRP(bp) ((char *)(bp) - WSIZE) //返回当前bp指向的块的header指针，bp需要是分配块的内容部分的首地址（不包括头部）
#define FTRP(bp) ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE) //返回当前分配块的footer指针
#define NEXT_BLKP(bp) ((char *)(bp) + GET_SIZE(((char *)(bp) - WSIZE))) //获取bp指向的块的下一个块的内容部分的地址
#define PREV_BLKP(bp) ((char *)(bp) - GET_SIZE(((char *)(bp) - DSIZE))) //获取bp指向的块的前一个块的内容部分的地址
//先计算bp的头部的位置，使用GET_SIZE宏，计算得到当前块的大小，由于bp指向当前块的header之后的首地址，因此加上size之后，实际指向的是下一个块的header之后的内容

//使用隐式链表法
static char *heap_listp;

static void *extend_heap(size_t words); //扩展
static void *find_fit(size_t size); //寻找合适的块
static void place(void *bp, size_t size); //切割块
static void *coalesce(void *bp); //合并

/* 
 * mm_init - initialize the malloc package.
 */
int mm_init(void)
{
    if((heap_listp = mem_sbrk(4 * WSIZE)) == (void *)-1){
        return -1;
    }
    PUT(heap_listp, 0);//alignment padding
    PUT(heap_listp + (1 * WSIZE), PACK(DSIZE, 1));//序言块header,序言块占两个字
    PUT(heap_listp + (2 * WSIZE), PACK(DSIZE, 1));//序言块footer
    PUT(heap_listp + (3 * WSIZE), PACK(0, 1));//结尾块，只占一个字，包含一个header，内容全0
    heap_listp += (2 * WSIZE); //序言块header之后的首地址，相当于序言块的data部分，但是序言块data为0，因此指向序言块的footer

    //extend
    if(extend_heap(CHUNKSIZE / WSIZE) == NULL){
        return -1;
    }
    return 0;
}

/* 
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size)
{
    size_t asize;
    size_t extendsize;
    char *bp;

    if(size == 0){
        return NULL;
    }

    if(size <= DSIZE){
        asize = 2 * DSIZE;
    }else{
        asize = DSIZE * ((size + (DSIZE) + (DSIZE - 1)) / DSIZE);
    }

    if((bp = find_fit(asize)) != NULL){
        place(bp, asize);//切割
        return bp;
    }
    
    //没有找到合适的块，扩展堆
    extendsize = MAX(asize, CHUNKSIZE);
    if((bp = extend_heap(extendsize / WSIZE))== NULL){
        return NULL;
    }
    place(bp, asize);
    return bp;
}

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *ptr)
{
    size_t size = GET_SIZE(HDRP(ptr));
    //初始化释放块的header和footer
    PUT(HDRP(ptr), PACK(size, 0));
    PUT(FTRP(ptr), PACK(size, 0));
    //合并
    coalesce(ptr);
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 * 当ptr为null时，等同malloc
 * 当size为0时，等同free
 */
void *mm_realloc(void *ptr, size_t size)
{
    if(ptr == NULL){
        return mm_malloc(size);
    }
    if(size == 0){
        mm_free(ptr);
        return NULL;
    }

    void *oldptr = ptr;
    void *newptr;
    size_t copySize;
    
    newptr = mm_malloc(size);
    if (newptr == NULL)
      return NULL;
    size = GET_SIZE(HDRP(ptr));
    copySize = GET_SIZE(HDRP(newptr));
    if (size < copySize)
      copySize = size;
    memcpy(newptr, oldptr, copySize - WSIZE);
    mm_free(ptr);
    return newptr;
}

//当堆初始化时会被调用
//当从堆中找不到合适的块时
//这两种情况会调用该函数,分配是从一个顺序的数组中再分配一个块
//序言块，分配块，分配块，结束块,未分配堆空间 ----> 序言块，分配块，分配块，分配块，结束块
static void* extend_heap(size_t words){
    char *bp;
    size_t size;

    //保持双字对齐
    size = (words % 2) ? (words + 1) * WSIZE : words * WSIZE;
    //long 为双字
    if((long)(bp = mem_sbrk(size)) == -1){
        return NULL;
    }

    //初始化空闲块的header和footer,初始时空闲块为一整个空闲块
    PUT(HDRP(bp), PACK(size, 0));
    PUT(FTRP(bp), PACK(size, 0));
    PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1));//更新新的结尾块

    return coalesce(bp);//当找不到合适的时候，分配了一个块，此时可能原来堆有空闲块，因此触发合并
}

//合并
static void* coalesce(void *bp){
    size_t prev = GET_ALLOC(FTRP(PREV_BLKP(bp)));//获取前一个块的footer
    size_t next = GET_ALLOC(HDRP(NEXT_BLKP(bp)));//获取bp的下一个块的头部，可能是结束块
    size_t size = GET_SIZE(HDRP(bp));

    //如果prev和next都是已分配的
    if(prev && next){
        return bp;
    }else if(prev && !next){
        //next为未分配的，当前块和next合并
        size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
        PUT(HDRP(bp), PACK(size, 0));
        PUT(FTRP(bp), PACK(size, 0));
    }else if(!prev && next){
        //prev是未分配的，当前块合并到prev中,此时将header变为prev的header，footer不变
        size += GET_SIZE(HDRP(PREV_BLKP(bp)));
        PUT(FTRP(bp), PACK(size, 0));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));//更新prev的header
        bp = PREV_BLKP(bp);//更新bp，指向prev
    }else{
        //前后都是空闲的,header使用prev的header，footer使用next的footer
        size += GET_SIZE(HDRP(PREV_BLKP(bp))) + GET_SIZE(HDRP(NEXT_BLKP(bp)));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        PUT(FTRP(NEXT_BLKP(bp)), PACK(size, 0));
        bp = PREV_BLKP(bp);
    }
    return bp;
}

static void *find_fit(size_t size){
    char *bp = heap_listp;//获取首块(序言块)的地址
    size_t alloc;
    size_t asize;
    //循环获取可分配块
    while(GET_SIZE(HDRP(NEXT_BLKP(bp))) > 0){
        bp = NEXT_BLKP(bp);
        alloc = GET_ALLOC(HDRP(bp));
        if(alloc){
            continue;
        }
        asize = GET_SIZE(HDRP(bp));
        if (asize < size){
            continue;
        }
        return bp;
    }
    return NULL;
}

//切割块
static void place(void *bp, size_t size){
    size_t asize = GET_SIZE(HDRP(bp));

    if((asize - size) >= (2 * DSIZE)){
        //当块的大小减去需要的size，依然大于两个字,小于两个DSIZE没有必要，无法成为一个新的分配块
        //将bp切成两块
        PUT(HDRP(bp), PACK(size, 1));
        PUT(FTRP(bp), PACK(size, 1));
        //形成一个新的块
        PUT(HDRP(NEXT_BLKP(bp)), PACK(asize - size, 0));
        PUT(HDRP(NEXT_BLKP(bp)), PACK(asize - size, 0));
    }else{
        //如果没有两个DSIZE，那么将当前块切分成两个块，剩余的块无法成为一个新的块
        PUT(HDRP(bp), PACK(asize, 1));
        PUT(FTRP(bp), PACK(asize, 1));
    }
}






