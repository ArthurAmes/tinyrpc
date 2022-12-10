/* 
    Author: Arthur Ames

    Helper Macros for the RPC_FUNC macro.
    All this is done so that everything that can be resolve at compile-time, is resolved at compile time.
    Also, it makes all of our RPC functions typesafe which is really nice.

*/

#ifndef _TINY_RPC_MACROS_H
#define _TINY_RPC_MACROS_H


/* Helper Macros */

#define EVAL(...) __VA_ARGS__
#define VARCOUNT_TUPLE(x) \
    EVAL(VARCOUNT x)
#define VARCOUNT(...) \
   EVAL(VARCOUNT_I(__VA_ARGS__,9,8,7,6,5,4,3,2,1,))
#define VARCOUNT_I(_,_9,_8,_7,_6,_5,_4,_3,_2,X_,...) X_
#define GLUE(X,Y) GLUE_I(X,Y)
#define GLUE_I(X,Y) X##Y
#define FIRST(...) EVAL(FIRST_I(__VA_ARGS__,))
#define FIRST_I(X,...) X
#define TUPLE_TAIL(...) EVAL(TUPLE_TAIL_I(__VA_ARGS__))
#define TUPLE_TAIL_I(X,...) (__VA_ARGS__)


/* Macros to expand sizeof addition */

#define MAPT_X_1(NAME_, ARGS_)
#define MAPT_X_2(NAME_, ARGS_) NAME_(FIRST ARGS_) MAPT_X_1(NAME_, TUPLE_TAIL ARGS_)
#define MAPT_X_3(NAME_, ARGS_) NAME_(FIRST ARGS_) MAPT_X_2(NAME_, TUPLE_TAIL ARGS_)
#define MAPT_X_4(NAME_, ARGS_) NAME_(FIRST ARGS_) MAPT_X_3(NAME_, TUPLE_TAIL ARGS_)
#define MAPT_X_5(NAME_, ARGS_) NAME_(FIRST ARGS_) MAPT_X_4(NAME_, TUPLE_TAIL ARGS_)
#define MAPT_X_6(NAME_, ARGS_) NAME_(FIRST ARGS_) MAPT_X_5(NAME_, TUPLE_TAIL ARGS_)
#define MAPT_X_7(NAME_, ARGS_) NAME_(FIRST ARGS_) MAPT_X_6(NAME_, TUPLE_TAIL ARGS_)
#define MAPT_X_8(NAME_, ARGS_) NAME_(FIRST ARGS_) MAPT_X_7(NAME_, TUPLE_TAIL ARGS_)
#define MAPT_X_9(NAME_, ARGS_) NAME_(FIRST ARGS_) MAPT_X_8(NAME_, TUPLE_TAIL ARGS_)

#define MAPT_XX(NAME_, ARGS_) GLUE(MAPT_XX_,VARCOUNT ARGS_)(NAME_, ARGS_)
#define MAPT_XX_1(NAME_, ARGS_) NAME_(FIRST ARGS_)
#define MAPT_XX_2(NAME_, ARGS_) NAME_(FIRST ARGS_) MAPT_XX_1(NAME_, TUPLE_TAIL ARGS_)
#define MAPT_XX_3(NAME_, ARGS_) NAME_(FIRST ARGS_) MAPT_XX_2(NAME_, TUPLE_TAIL ARGS_)
#define MAPT_XX_4(NAME_, ARGS_) NAME_(FIRST ARGS_) MAPT_XX_3(NAME_, TUPLE_TAIL ARGS_)
#define MAPT_XX_5(NAME_, ARGS_) NAME_(FIRST ARGS_) MAPT_XX_4(NAME_, TUPLE_TAIL ARGS_)
#define MAPT_XX_6(NAME_, ARGS_) NAME_(FIRST ARGS_) MAPT_XX_5(NAME_, TUPLE_TAIL ARGS_)
#define MAPT_XX_7(NAME_, ARGS_) NAME_(FIRST ARGS_) MAPT_XX_6(NAME_, TUPLE_TAIL ARGS_)
#define MAPT_XX_8(NAME_, ARGS_) NAME_(FIRST ARGS_) MAPT_XX_7(NAME_, TUPLE_TAIL ARGS_)
#define MAPT_XX_9(NAME_, ARGS_) NAME_(FIRST ARGS_) MAPT_XX_8(NAME_, TUPLE_TAIL ARGS_)

/* Macro to do sizeof expansion */

#define MAP(NAME_, ARGS_) (GLUE(MAPT_,VARCOUNT ARGS_)(NAME_, ARGS_))
#define MAPT_1(NAME_, ARGS_, ...) *(FIRST ARGS_*) (NAME_ GLUE(MAPT_X_,VARCOUNT_TUPLE ((__VA_ARGS__))) (+ sizeof, (__VA_ARGS__)))
#define MAPT_2(NAME_, ARGS_, ...) *(FIRST ARGS_*) (NAME_ GLUE(MAPT_X_,VARCOUNT_TUPLE ((__VA_ARGS__))) (+ sizeof, (__VA_ARGS__))), MAPT_1(NAME_,TUPLE_TAIL ARGS_,FIRST ARGS_,__VA_ARGS__)
#define MAPT_3(NAME_, ARGS_, ...) *(FIRST ARGS_*) (NAME_ GLUE(MAPT_X_,VARCOUNT_TUPLE ((__VA_ARGS__))) (+ sizeof, (__VA_ARGS__))), MAPT_2(NAME_,TUPLE_TAIL ARGS_,FIRST ARGS_,__VA_ARGS__)
#define MAPT_4(NAME_, ARGS_, ...) *(FIRST ARGS_*) (NAME_ GLUE(MAPT_X_,VARCOUNT_TUPLE ((__VA_ARGS__))) (+ sizeof, (__VA_ARGS__))), MAPT_3(NAME_,TUPLE_TAIL ARGS_,FIRST ARGS_,__VA_ARGS__)
#define MAPT_5(NAME_, ARGS_, ...) *(FIRST ARGS_*) (NAME_ GLUE(MAPT_X_,VARCOUNT_TUPLE ((__VA_ARGS__))) (+ sizeof, (__VA_ARGS__))), MAPT_4(NAME_,TUPLE_TAIL ARGS_,FIRST ARGS_,__VA_ARGS__)
#define MAPT_6(NAME_, ARGS_, ...) *(FIRST ARGS_*) (NAME_ GLUE(MAPT_X_,VARCOUNT_TUPLE ((__VA_ARGS__))) (+ sizeof, (__VA_ARGS__))), MAPT_5(NAME_,TUPLE_TAIL ARGS_,FIRST ARGS_,__VA_ARGS__)
#define MAPT_7(NAME_, ARGS_, ...) *(FIRST ARGS_*) (NAME_ GLUE(MAPT_X_,VARCOUNT_TUPLE ((__VA_ARGS__))) (+ sizeof, (__VA_ARGS__))), MAPT_6(NAME_,TUPLE_TAIL ARGS_,FIRST ARGS_,__VA_ARGS__)
#define MAPT_8(NAME_, ARGS_, ...) *(FIRST ARGS_*) (NAME_ GLUE(MAPT_X_,VARCOUNT_TUPLE ((__VA_ARGS__))) (+ sizeof, (__VA_ARGS__))), MAPT_7(NAME_,TUPLE_TAIL ARGS_,FIRST ARGS_,__VA_ARGS__)
#define MAPT_9(NAME_, ARGS_, ...) *(FIRST ARGS_*) (NAME_), MAPT_8(NAME_,TUPLE_TAIL ARGS_, FIRST ARGS_, __VA_ARGS__)

/* Macros to expand function definition arguments */

#define FN_DEF_EXPAND(ARGS_) GLUE(MAPN_,VARCOUNT ARGS_)(ARGS_)
#define MAPN_1(ARGS_) FIRST ARGS_ i
#define MAPN_2(ARGS_) FIRST ARGS_ h,MAPN_1(TUPLE_TAIL ARGS_)
#define MAPN_3(ARGS_) FIRST ARGS_ g,MAPN_2(TUPLE_TAIL ARGS_)
#define MAPN_4(ARGS_) FIRST ARGS_ f,MAPN_3(TUPLE_TAIL ARGS_)
#define MAPN_5(ARGS_) FIRST ARGS_ e,MAPN_4(TUPLE_TAIL ARGS_)
#define MAPN_6(ARGS_) FIRST ARGS_ d,MAPN_5(TUPLE_TAIL ARGS_)
#define MAPN_7(ARGS_) FIRST ARGS_ c,MAPN_6(TUPLE_TAIL ARGS_)
#define MAPN_8(ARGS_) FIRST ARGS_ b,MAPN_7(TUPLE_TAIL ARGS_)
#define MAPN_9(ARGS_) FIRST ARGS_ a,MAPN_8(TUPLE_TAIL ARGS_)

/* Macros to expand packTransmit arguments */

#define PTX_EXPAND(ARGS_) GLUE(MAPN_X_,VARCOUNT ARGS_)
#define MAPN_X_1 sizeof(i),*(uint32_t*)(&i)
#define MAPN_X_2 sizeof(h),*(uint32_t*)(&h),MAPN_X_1
#define MAPN_X_3 sizeof(g),*(uint32_t*)(&g),MAPN_X_2
#define MAPN_X_4 sizeof(f),*(uint32_t*)(&f),MAPN_X_3
#define MAPN_X_5 sizeof(e),*(uint32_t*)(&e),MAPN_X_4
#define MAPN_X_6 sizeof(d),*(uint32_t*)(&d),MAPN_X_5
#define MAPN_X_7 sizeof(c),*(uint32_t*)(&c),MAPN_X_6
#define MAPN_X_8 sizeof(b),*(uint32_t*)(&b),MAPN_X_7
#define MAPN_X_9 sizeof(a),*(uint32_t*)(&a),MAPN_X_8

/* These macros are used to compare if the type of a variable is void */
/* Long story short, IS_VOID(x) evaluates to 1 if x is void, and 0 otherwise */

#define CHECK_N(x, n, ...) n
#define CHECK(...) CHECK_N(__VA_ARGS__, 0, )
#define PROBE(x) x, 1

#define VOID_CHECK__void

#define IS_VOID_PROBE(val)          IS_VOID_PROBE_PROXY(VOID_CHECK_##val)
#define IS_VOID_PROBE_PROXY(...)    IS_VOID_PROBE_PRIMI(__VA_ARGS__)
#define IS_VOID_PROBE_PRIMI(x)      IS_VOID_PROBE_COMBI_##x
#define IS_VOID_PROBE_COMBI_        PROBE(~)

#define IS_VOID(x) CHECK(IS_VOID_PROBE(x))


#endif