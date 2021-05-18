#include <falloc.h>

#include <inttypes.h>
#include <stdlib.h>


static void
_fcontext_stack_free_malloc(stack_context_t* sc) {
    void *ptr = (char *) (sc->sp) - sc->size;
    assert(sc != NULL);    
    free(ptr);
}

void*
fcontext_stack_malloc(stack_context_t* sc, size_t size) {
    char* ptr = malloc(size);
    assert(sc != NULL);
    assert(size >= FCONTEXT_SP_MINSIZE);
    if (ptr) {
        sc->sp = ptr + size;
        sc->size = size;
        sc->free = _fcontext_stack_free_malloc;
    } else {
        sc->sp = NULL;
        sc->size = 0;
        sc->free = NULL;
    }
    return sc->sp;
}
