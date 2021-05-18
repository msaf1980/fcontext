fcontext
==

Cross platform context switching library (based on boost::context).

API
--------------------
Context switching with `fcontext/fcontext.h`

```
fcontext_t make_fcontext(void * sp, size_t size, void(*fn)(transfer_t))
transfer_t jump_fcontext(fcontext_t const to, void *vp)
transfer_t ontop_fcontext(fcontext_t const to, void *vp, transfer_t(*fn)(transfer_t))
```

Stack alloc with `fcontext/falloc.h`

```
void* fcontext_stack_malloc(stack_context_t* sc, size_t size)
void  fcontext_stack_alloca(stack_context_t* sc, size_t size)  /* It's a macros, but argument types choosen */
void  fcontext_stack_free(stack_context_t* sc)                 /* It's a macros, but argument types choosen */
```

Third Party Software
--------------------

fcontext is based on code originating in [boost::context]

[boost::context]:   https://github.com/boostorg/context     "boostorg/context"
