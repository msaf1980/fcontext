#ifndef _FALLOC_H_
#define _FALLOC_H_

#include <assert.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef struct stack_context_s stack_context_t;
typedef void (*fcontext_stack_free_func)(stack_context_t *sc);

struct stack_context_s {
    void *sp; /* stack pointer (end of array/allocated block) */
    size_t size;
    fcontext_stack_free_func free;
};

#define FCONTEXT_SP_MINSIZE 2046

/* allocate stack with malloc */
void* fcontext_stack_malloc(stack_context_t* sc, size_t size);

#define fcontext_stack_alloca(sc, _size)                 \
    do {                                                \
        char* ptr = alloca(_size);                       \
        assert((_size) >= FCONTEXT_SP_MINSIZE);          \
        if (ptr) {                                      \
            (sc)->sp = ptr + _size;                      \
            (sc)->size = _size;                          \
        } else {                                        \
            (sc)->sp = NULL;                            \
            (sc)->size = 0;                             \
        }                                               \
        (sc)->free = NULL; /* allocated on stack */     \
    } while(0);

#define fcontext_stack_free(sc)    \
    do {                           \
        assert(sc != NULL);        \
        if ((sc)->free) {          \
            (sc)->free((sc));      \
        }                          \
    } while(0);

#ifdef __cplusplus
}
#endif

#endif //!_FALLOC_H_
