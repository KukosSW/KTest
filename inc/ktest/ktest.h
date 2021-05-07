#ifndef KTEST_FRAMEWORK_H
#define KTEST_FRAMEWORK_H

/*
    This is the main header of the Ktest framework, you shall include only this header for testing
    Author: Michal Kukowski
    email: michalkukowski10@gmail.com

    LICENCE: GPL3

    KTest = Kukos's (LightWeight) test framework developed in C for testing C code.
    KTest is compatible with gnu11 and every standards above C11 with gnu extension.
    You can develop your project without gnu extension.
    Just to be sure that tests have enabled gnu extension in Makefile.
    You can use gcc or clang or every other compiler compatible with gnu extension for compiling tests with KTest.

    HOW TO USE?
    1. Check examples :)
    2. INIT your test suite by using
        TEST_SUITE_INIT("My test name") or TEST_SUITE_INIT_WITH_FUNC("My test name", tc_init, tc_clean)
    3. Prepare test cases, this framework supports all kind of functions as test case,
        but please note, that framework wont use returned value.
        KTest supports also parametrized test case, you can create function with params like void f(int n).
        To test your functions (test case) pass function with parameters to macro TEST_CASE_RUN
        i.e TEST_CASE_RUN(f(10)), TEST_CASE_RUN(g())
    4. Finish your test suite to get statistics from KTest metadata using TEST_SUITE_SUMMARY()
    5. I suggest to return test suite status as a main return code, this framework supporst POSIX convection
        test suite status is equal 0 when entire suite pass  and non 0 value when suite failed.

    To create test cases you need a scenario wrapped into some function (with or without arguments).
    You need also a some assertions (checkpoints), to archive that you shall use
    T_EXPECT or T_ASSERT family.
    Please see macro description for better explanation.

    Framework supports multi file test suite, so you can split yours test suite to a few files,
    I suggest design as follows:
    test.c -> main file with TEST_SUITE_INIT and TEST_CASE_RUN + TEST_SUITE_SUMMARY
    test_subfile1.c -> file with test cases (functions with T_EXPECT / T_ASSERT)
    ...
    test_subfilen.c -> ....
    You can also put all code into 1 file like in an example, is not a problem. This is your decision.
    Please note that you can run several suites in 1 binary file. See examples for better understanding
*/

#include "ktest-priv.h"

/**
 *  Types for user (you can declare your own type, but function will be casted to this)
 */
typedef void tc_ret_t;


/**
 * Use this macro once per test suite, it will init all needed data
 * and print on stderr testsuite name in pretty format
 */
#define TEST_SUITE_INIT(suite_name) KTEST_TEST_SUITE_INIT(suite_name, NULL, NULL)

/**
 * Use this macro once per test suite, it will init all needed data
 * and print on stderr testsuite name in pretty format
 *
 * It will also set init and cleanup functions.
 * Init function will be called before every testcase
 * Cleanup function will be called after every testcase (if testcase ends)
 */
#define TEST_SUITE_INIT_WITH_FUNC(suite_name, tc_init, tc_clean) KTEST_TEST_SUITE_INIT(suite_name, tc_init, tc_clean)

/**
 * Use this macro once per test suite, it will finish test suite
 * and print on stderr all statistics about given testsuite
 */
#define TEST_SUITE_SUMMARY() KTEST_TEST_SUITE_SUMMARY()

/**
 * Use this macro to call passed function as a testcase.
 * TestCase should be tc_ret_t, but you can pass every function,
 * it can of another types returned value will be skipped.
 *
 * You can also preapre a parametrized test case,
 * this function supports functions with arguments
 *
 * use it as follows:
 * TEST_CASE_RUN(f());
 * TEST_CASE_RUN(g(100, 20, "some string"));
 */
#define TEST_CASE_RUN(function) KTEST_TEST_CASE_RUN(function)

/**
 * Use this macro to get test suite result in POSIX form.
 * 0 means test suite passed
 * >0 means test suite failed with X errors (X is the returned value)
 *
 * Please note that this functions remember only 1 testsuite
 * If you start another then prev value will be overwritten.
 * You can use this function after SUMMARY, because till another init
 * value will be keeped
 */
#define TEST_SUITE_GET_RESULT() KTEST_SUITE_GET_RESULT()

/**
 * Use this macros to check relation between 2 values
 * Macro supports functions, so you can pass non-void function as a value
 *
 * Also macros have compile time checking for types,
 * so rember to pass / cast values properly
 *
 * Relations:
 * EQ  -> val1 == val2
 * NEQ -> val1 != val2
 * LT  -> val1 <  val2
 * LEQ -> val1 <= val2
 * GT  -> val1 >  val2
 * GEQ -> val1 >= val2
 *
 * If condition (relation) is evaluated to false then test case
 * is marked as FAILED, on stderr proper print will be showed
 * and cpu will continue executing your funstion
 */
#define T_EXPECT_EQ(val1, val2)    KTEST_EXPECT_EQ(val1, val2)
#define T_EXPECT_NEQ(val1, val2)   KTEST_EXPECT_NEQ(val1, val2)
#define T_EXPECT_GT(val1, val2)    KTEST_EXPECT_GT(val1, val2)
#define T_EXPECT_GEQ(val1, val2)   KTEST_EXPECT_GEQ(val1, val2)
#define T_EXPECT_LT(val1, val2)    KTEST_EXPECT_LT(val1, val2)
#define T_EXPECT_LEQ(val1, val2)   KTEST_EXPECT_LEQ(val1, val2)

/**
 * This macro is similar to T_EXCEPT_
 * but works with more complicated conditions.
 *
 * The main drawback is the print. Framework cannot split conditions to tokens
 * and chose proper printf format, so print will be "ugly"
 * Thats why I suggest to split code into several conditions.
 *
 * You can use this macro like this:
 * T_EXPECT(list != NULL && (list->head == NULL || list->size == 0))
 *
 */
#define T_EXPECT(cond)             KTEST_EXPECT_COND(cond)

/**
 * Use this macro to check if pointer is not null
 */
#define T_EXPECT_PTR_NOT_NULL(ptr) T_EXPECT_NEQ(ptr, (void *)0)

/**
 * Use this macro to check if pointer is null
 */
#define T_EXPECT_PTR_NULL(ptr)     T_EXPECT_EQ(ptr, (void *)0)

/**
 * Those macros have the same functionality as T_EXCEPT
 * with 1 different thing.
 * When condition failed then cpu will stop executing your function
 * But wont stop entire test suite
 */
#define T_ASSERT_EQ(val1, val2)    KTEST_ASSERT_EQ(val1, val2)
#define T_ASSERT_NEQ(val1, val2)   KTEST_ASSERT_NEQ(val1, val2)
#define T_ASSERT_GT(val1, val2)    KTEST_ASSERT_GT(val1, val2)
#define T_ASSERT_GEQ(val1, val2)   KTEST_ASSERT_GEQ(val1, val2)
#define T_ASSERT_LT(val1, val2)    KTEST_ASSERT_LT(val1, val2)
#define T_ASSERT_LEQ(val1, val2)   KTEST_ASSERT_LEQ(val, val2)
#define T_ASSERT(cond)             KTEST_ASSERT_COND(cond)
#define T_ASSERT_PTR_NOT_NULL(ptr) T_ASSERT_NEQ(ptr, (void *)0)
#define T_ASSERT_PTR_NULL(ptr)     T_ASSERT_EQ(ptr, (void *)0)

#endif
