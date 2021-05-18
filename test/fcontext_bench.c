#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#include <fcontext/fcontext.h>
#include <fcontext/falloc.h>

#include "benchmark.h"

#define NUM_CONTEXT 10000
#define NUM_LOOP 300

uint64_t jobs = 10000000;

static void foo(transfer_t t) {
    transfer_t t_ = t;
    while (1) {
        t_ = jump_fcontext(t_.fctx, NULL);
    }
}

double measure_time_jump_fcontext() {
    uint64_t i;
    stack_context_t sp;
    fcontext_t ctx;
    fcontext_stack_alloca(&sp, 4096);
    ctx = make_fcontext(sp.sp, sp.size, foo);
    unsigned long long start;
    double duration;

    // cache warum-up
    transfer_t t = jump_fcontext(ctx, NULL);

    start = clock_ns();
    for (i = 0; i < jobs; ++i) {
        t = jump_fcontext(t.fctx, NULL);
    }
    /* 2x jump_fcontext */    
    duration = (double) (clock_ns() - start) / (jobs * 2);

    return duration;
}


int main()
{
    double duration;

    duration = measure_time_jump_fcontext();
    printf("jump_fcontext: average of %.2f nano seconds\n", duration);

    return EXIT_SUCCESS;
}
