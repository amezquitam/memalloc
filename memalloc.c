#include "memalloc.h"

typedef struct memblockfree {
    void* block;
    size_t size;
    struct memblockfree* next_free;
} memblockfree;

static memblockfree* shmarena;

void memalloc_init(void *arena, size_t arena_size) {
    memblockfree *first = (memblockfree*) arena;
    first->block = arena + sizeof(memblockfree);
    first->size = arena_size - sizeof(memblockfree);
    first->next_free = NULL;
    shmarena = first;
}

void *memalloc(size_t size) {
    size += sizeof(size_t);
    if (size < sizeof(memblockfree)) return NULL;
    void* block = NULL;
    memblockfree* it = shmarena;
    while (it != NULL) {
        if (it->size > size) {
            block = it->block + it->size - size;
            it->size -= size;
            break;
        }
        it = it->next_free;
    }
    if (block == NULL) return NULL;
    *((size_t*) block) = size;
    return block + sizeof(size_t);
}

void memfree(void *mem) {
    mem -= sizeof(size_t);
    size_t size = *((size_t*) mem);
    memblockfree* new_free = (memblockfree*) mem;
    new_free->block = mem + sizeof(memblockfree);
    new_free->size = size - sizeof(memblockfree);
    new_free->next_free = NULL;
    memblockfree* it = shmarena, *pt;
    while (it->next_free != NULL) {
        it = it->next_free;
    }
    it->next_free = new_free;

    // -- remove continuous free blocks
    it = shmarena;
    int next;
    while (it != NULL) {
        next = 1;
        void* tar = it->block + it->size;
        pt = shmarena;
        while (pt->next_free != NULL) {
            memblockfree* npt = pt->next_free;
            if (tar == (void*) npt) {
                it->size += npt->size + sizeof(memblockfree);
                pt->next_free = npt->next_free;
                next = 0;
                break;
            }
            pt = pt->next_free;
        }
        if (next)
            it = it->next_free;
    }
}