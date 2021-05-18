/*
 * Copyright (c) 2016 Johan Sköld
 * License: https://opensource.org/licenses/ISC
 */

/*
 * This sample implements a simple fibonacci context whose only purpose is to
 * repeatedly yield the next value in the fibonacci sequence. By repeatedly
 * switching to the fibonacci context, its parent context (the context that
 * created it, in this case the implicitly created main context) will receive
 * the next number in the sequence.
 *
 *   cc -I../include -L../build/lib fibonacci.c -lsc -o fibonacci
*/

#include <fcontext/fcontext.h>
#include <fcontext/falloc.h>

#include <stdlib.h> /* abort */
#include <stdio.h> /* printf */

#define CTEST_MAIN
#define CTEST_SEGFAULT

#include "ctest.h"

fcontext_t fc1, fc2;

int value1 , value2;

void ontop_f1(transfer_t t_) {
    printf("f1: entered\n");    
    transfer_t t = jump_fcontext(t_.fctx, t_.data);
    value1 = * (int *) t.data;
    jump_fcontext(t.fctx, t.data);
}

transfer_t ontop_f2(transfer_t t_) {
    transfer_t t = { t_.fctx, t_.data };
    printf("f2: entered\n");
    value2 = * (int *) t_.data;    
    return t;
}

CTEST(fcontext, ontop) {
    stack_context_t sp;
    fcontext_t ctx;
    transfer_t t;
    int i = -3;
    value1 = 1;
    value2 = 0;
    fcontext_stack_alloca(&sp, 4096);
    ctx = make_fcontext(sp.sp, sp.size, ontop_f1);
    t = jump_fcontext(ctx, NULL);
    ASSERT_EQUAL(1, value1);
    ASSERT_EQUAL(0, value2);
    t = ontop_fcontext(t.fctx, &i, ontop_f2);
    ASSERT_EQUAL(-3, value1);
    ASSERT_EQUAL(-3, value2);
    ASSERT_EQUAL(-3, * (int *) t.data);
    ASSERT_EQUAL((intptr_t) &i, (intptr_t) t.data);
}

void stacked_f2(transfer_t t);
void stacked_f1(transfer_t t) {
    stack_context_t sp;
    fcontext_t ctx;

    printf("f1: entered\n");
    fcontext_stack_malloc(&sp, 4096); /* don't use stack allocated for stacked context, alloc in heap or other  */
    ctx = make_fcontext(sp.sp, sp.size, stacked_f2);
    jump_fcontext(ctx, t.data);
    printf("f1: return to main\n");
    jump_fcontext(t.fctx, t.data);
    fcontext_stack_free(&sp);
}

void stacked_f2(transfer_t t) {
    int *value = (int*) t.data;
    printf("f2: entered\n");
    *value = 3;
    jump_fcontext(t.fctx, t.data);
    fprintf(stderr, "f2: never returns");
    fflush(stdout);
    fflush(stderr);
    abort();
}

CTEST(fcontext, stacked) {
    int value = 0;
    stack_context_t sp;
    fcontext_t ctx;
    fcontext_stack_alloca(&sp, 4096);
    printf("\n");
    ctx = make_fcontext(sp.sp, sp.size, stacked_f1);
    jump_fcontext(ctx, &value);
    ASSERT_EQUAL(3, value);
}

void jump_f1(transfer_t t)
{
    int *value = (int*) t.data;

    printf("f1: entered\n");

    ASSERT_EQUAL(0, *value);
    *value = 1;

    printf("f1: call jump_fcontext(fc2, NULL)\n");

    jump_fcontext(fc2, t.data);

    ASSERT_EQUAL(2, *value);
    (*value)++;
    ASSERT_EQUAL(3, *value);

    printf("f1: return to main\n");
    jump_fcontext(t.fctx, t.data);
}

void jump_f2(transfer_t t)
{
    int *value = (int*) t.data;

    printf("f2: entered\n");

    ASSERT_EQUAL(1, *value);
    *value = 2;
    ASSERT_EQUAL(2, *value);

    printf("f2: call jump_fcontext(fc1, NULL)\n");
    jump_fcontext(t.fctx, t.data);
    fprintf(stderr, "f2: never returns");
    fflush(stdout);
    fflush(stderr);
    abort();
}

CTEST(fcontext, jump) {
    int value = 0;
    size_t size = 4096;
    stack_context_t sp1, sp2;
    fcontext_stack_alloca(&sp1, size);
    fcontext_stack_malloc(&sp2, size);

    fc1 = make_fcontext(sp1.sp, sp1.size, jump_f1);
    fc2 = make_fcontext(sp2.sp, sp2.size, jump_f2);

    printf("\nmain: call jump_fcontext(&fcm, fc1, NULL)\n");
    jump_fcontext(fc1, &value);

    printf("main: done\n");

    ASSERT_EQUAL(3, value);

    fcontext_stack_free(&sp1);
    fcontext_stack_free(&sp2);
}

CTEST(fcontext, setup) {
    size_t size = 4096;
    stack_context_t sp1;
    fcontext_stack_alloca((&sp1), size);

    fc1 = make_fcontext(sp1.sp, sp1.size, jump_f1);

    fcontext_stack_free(&sp1);
}

int main(int argc, const char *argv[])
{
    int result = ctest_main(argc, argv);

    return result;
}
