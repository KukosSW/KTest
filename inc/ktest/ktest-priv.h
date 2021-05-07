#ifndef KTEST_PRIV_H
#define KTEST_PRIV_H

/*
    This is the private header for the ktest framework.
    Do not include it directly

    Author: Michal Kukowski
    email: michalkukowski10@gmail.com

    LICENCE: GPL3
*/

#ifndef KTEST_FRAMEWORK_H
#error "Never include <ktest/ktest-priv.h> directly, use <ktest/ktest.h> instead."
#endif

#ifndef __GNUC__
#error "Gnu extension is required to compile tests with KTest!"
#endif

/* C11 has value 201112L */
#if __STDC_VERSION__ < 201112L
#error "At least C11 is required to compile tests with KTest!"
#endif

/* all what we and user also need, because we are using something from this libs in macros */
#include <stddef.h>
#include <stdbool.h>

#include "ktest-terminal-colors.h"
#include "ktest-primitive-fmt.h"
#include "ktest-diag.h"

#ifndef TOSTRING
#define KTEST_TOSTRING(x) #x
#define TOSTRING(x) KTEST_TOSTRING(x)
#endif

/* New types */
typedef void (*ktest_test_case_init_f)(void);
typedef void (*ktest_test_case_clean_f)(void);

typedef unsigned long ktest_tests_counter_t;
typedef enum
{
    KTEST_TEST_RESULT_PASSED,
    KTEST_TEST_RESULT_FAILED,
} ktest_test_result_t;

typedef enum
{
    KTEST_FRAMEWORK_UNINITIALIZED,
    KTEST_FRAMEWORK_STARTED,
} ktest_framework_states_t;

/* Global functions */

/* This function is only a wrapper for fprintf, so use proper printf format */
void __ktest_print(const char* fmt, ...);

/* INIT / SUMMARY (FINISH), call only once for each testsuite */
void __ktest_init(const char* msg, ktest_test_case_init_f tc_init, ktest_test_case_clean_f tc_clean);
void __ktest_summary(void);

/* Returns 0 if suite passed, number of errors if suite failed  */
int __ktest_get_suite_result(void);

/* Returns pointer to char[] with printf format */
const char* __ktest_create_assert_print_fmt(const char* label,
                                            const char* val1_fmt,
                                            const char* op,
                                            const char* val2_fmt);

/* GET/SET */
ktest_test_result_t __ktest_tc_result_get(void);
void __ktest_tc_result_set(ktest_test_result_t res);

/* Call before and after test case */
void __ktest_test_case_run_finish(const char* testname);
void __ktest_test_case_run_prepare(const char* testname);

/* PRIVATE MACROS */
#define KTEST_TEST_SUITE_INIT(fmt, tc_init, tc_clean) \
    do { \
        __ktest_init(fmt, tc_init, tc_clean); \
    } while (0)

#define KTEST_TEST_SUITE_SUMMARY() \
    do { \
        __ktest_summary(); \
    } while(0)


/* This macro call another macro with TC testing, if some assert failed finish TC */
#define KTEST_RETURN_IF_ERROR(TMACRO) \
    do { \
        TMACRO; \
        if (__ktest_tc_result_get() == KTEST_TEST_RESULT_FAILED) \
        { \
            return; \
        } \
    } while (0)

#define KTEST_EXPECT_OR_ASSERT_COND(cond, label) \
    do { \
        if (!(cond)) \
        { \
            __ktest_print("[" label "] %s:%d\t%s\n", __FILE__, __LINE__, TOSTRING(cond)); \
            __ktest_tc_result_set(KTEST_TEST_RESULT_FAILED); \
        } \
    } while (0)

/*
    When val1 op val2 is false then mark test case as failed and print values

    Macro is using strict type checking with some exceptions:
    1. If both types are pointers then pointer are compatible. Because for comparision we can use just void*
    2. If one of the variable is a bool variable then
        2.1 Second val is a variable, then make type checking
        2.2 Second val is a const value, then check if can be converted to bool (is equal 0 or 1)
        2.3 First value is a const value and second value is a const value, they cannot by bool, because of promotion to int

    Plese note that _Static_assert need all of branches be compile expressions.
    That's why I use val1 / val2 instead of _ktest_val. But no worries. __typeof__ / sizeof are safe
    ++i, i++ will be ignored under those compiler "operators"
 */
#define KTEST_EXPECT_OR_ASSERT_OP(val1, val2, op, label) \
    do { \
            KTEST_DIAG_PUSH() \
            KTEST_DIAG_IGNORE("-Wfloat-equal") \
            KTEST_DIAG_IGNORE("-Wint-to-pointer-cast") \
            const  __typeof__(val1) _ktest_val1 = (val1); \
            const  __typeof__(val2) _ktest_val2 = (val2); \
            _Static_assert(__builtin_choose_expr(KTEST_PRIMITIVES_PROBABLY_POINTER(_ktest_val1) && KTEST_PRIMITIVES_PROBABLY_POINTER(_ktest_val2), \
                                                 1, \
                                                 __builtin_choose_expr(KTEST_PRIMITIVE_GET_TYPE(_ktest_val1) == KTEST_PRIMITIVES_BOOL || KTEST_PRIMITIVE_GET_TYPE(_ktest_val2) == KTEST_PRIMITIVES_BOOL, \
                                                                       1, \
                                                                       __builtin_types_compatible_p(__typeof__(val1), __typeof__(val2)))), \
                           "Uncompatible types"); \
            _Static_assert(__builtin_choose_expr(KTEST_PRIMITIVE_GET_TYPE(_ktest_val1) == KTEST_PRIMITIVES_BOOL || KTEST_PRIMITIVE_GET_TYPE(_ktest_val2) == KTEST_PRIMITIVES_BOOL, \
                                                 __builtin_choose_expr(__builtin_constant_p(val1), \
                                                                       val1 == (__typeof__(val1))false && val1 == (__typeof__(val1))true, \
                                                                       __builtin_choose_expr(__builtin_constant_p(val2), \
                                                                                             val2 == (__typeof__(val2))false || val2 == (__typeof__(val2))true, \
                                                                                             1) \
                                                                       ), \
                                                 1), \
                           "Implicit convertion to bool"); \
            if (!((__builtin_choose_expr(KTEST_PRIMITIVES_PROBABLY_POINTER(_ktest_val1), (void *)(long)_ktest_val1, _ktest_val1)) op (__builtin_choose_expr(KTEST_PRIMITIVES_PROBABLY_POINTER(_ktest_val2), (void *)(long)_ktest_val2, _ktest_val2)))) \
            { \
                const char* const fmt_ptr = \
                    __ktest_create_assert_print_fmt(label, \
                                                    KTEST_PRIMTIVE_GET_FMT(_ktest_val1), \
                                                    TOSTRING(op), \
                                                    KTEST_PRIMTIVE_GET_FMT(_ktest_val2)); \
                __ktest_print(fmt_ptr, \
                              __FILE__, \
                              __LINE__, \
                              _ktest_val1, \
                              _ktest_val2); \
                __ktest_tc_result_set(KTEST_TEST_RESULT_FAILED); \
            } \
            KTEST_DIAG_POP() \
    } while (0)

/* Relation set: ==, !=, < ... */
#define KTEST_EXPECT_EQ(val1, val2)  KTEST_EXPECT_OR_ASSERT_OP(val1, val2, ==, "T_EXPECT")
#define KTEST_EXPECT_NEQ(val1, val2) KTEST_EXPECT_OR_ASSERT_OP(val1, val2, !=, "T_EXPECT")
#define KTEST_EXPECT_LT(val1, val2)  KTEST_EXPECT_OR_ASSERT_OP(val1, val2, <, "T_EXPECT")
#define KTEST_EXPECT_LEQ(val1, val2) KTEST_EXPECT_OR_ASSERT_OP(val1, val2, <=, "T_EXPECT")
#define KTEST_EXPECT_GT(val1, val2)  KTEST_EXPECT_OR_ASSERT_OP(val1, val2, >, "T_EXPECT")
#define KTEST_EXPECT_GEQ(val1, val2) KTEST_EXPECT_OR_ASSERT_OP(val1, val2, >=, "T_EXPECT")
#define KTEST_EXPECT_COND(cond)      KTEST_EXPECT_OR_ASSERT_COND(cond, "T_EXPECT")

#define KTEST_ASSERT_EQ(val1, val2)  KTEST_RETURN_IF_ERROR(KTEST_EXPECT_OR_ASSERT_OP(val1, val2, ==, "T_ASSERT"))
#define KTEST_ASSERT_NEQ(val1, val2) KTEST_RETURN_IF_ERROR(KTEST_EXPECT_OR_ASSERT_OP(val1, val2, !=, "T_ASSERT"))
#define KTEST_ASSERT_LT(val1, val2)  KTEST_RETURN_IF_ERROR(KTEST_EXPECT_OR_ASSERT_OP(val1, val2, <, "T_ASSERT"))
#define KTEST_ASSERT_LEQ(val1, val2) KTEST_RETURN_IF_ERROR(KTEST_EXPECT_OR_ASSERT_OP(val1, val2, <=, "T_ASSERT"))
#define KTEST_ASSERT_GT(val1, val2)  KTEST_RETURN_IF_ERROR(KTEST_EXPECT_OR_ASSERT_OP(val1, val2, >, "T_ASSERT"))
#define KTEST_ASSERT_GEQ(val1, val2) KTEST_RETURN_IF_ERROR(KTEST_EXPECT_OR_ASSERT_OP(val1, val2, >=, "T_ASSERT"))
#define KTEST_ASSERT_COND(cond)      KTEST_RETURN_IF_ERROR(KTEST_EXPECT_OR_ASSERT_COND(cond, "T_ASSERT"))


/* Use this macto to test function */
#define KTEST_TEST_CASE_RUN(tc) \
    do { \
        __ktest_test_case_run_prepare(TOSTRING(tc)); \
        (void)tc; \
        __ktest_test_case_run_finish(TOSTRING(tc)); \
    } while (0)

#define KTEST_SUITE_GET_RESULT() __ktest_get_suite_result()

#endif
