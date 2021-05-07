#include <stdarg.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <ktest/ktest.h>

#define KTEST_COLOR_INFO          COLOR_CYAN
#define KTEST_COLOR_PASSED        COLOR_GREEN
#define KTEST_COLOR_FAILED        COLOR_RED
#define KTEST_PRINT_INFO(fmt)     KTEST_COLOR_INFO fmt COLOR_RESET
#define KTEST_PRINT_PASSED(fmt)   KTEST_COLOR_PASSED fmt COLOR_RESET
#define KTEST_PRINT_FAILED(fmt)   KTEST_COLOR_FAILED fmt COLOR_RESET

/***** STATIC VARIABLES *****/
static ktest_test_case_init_f     ktest_test_case_init; /* call before each TC */
static ktest_test_case_clean_f    ktest_test_case_clean; /* call after each TC */
static ktest_tests_counter_t      ktest_tests_passed_counter;
static ktest_tests_counter_t      ktest_tests_failed_counter;
static ktest_test_result_t        ktest_test_case_result;
static ktest_framework_states_t  ktest_framework_state = KTEST_FRAMEWORK_UNINITIALIZED;

#define KTEST_STRING_MAX_LEN 60
static char ktest_carray_spaces[KTEST_STRING_MAX_LEN];
static char ktest_carray_equals[KTEST_STRING_MAX_LEN];
static char ktest_static_buffer[1024]; /* Pr[string > 1024] -> 0, so lets keep this as is without dynamic allocation */

/***** STATIC FUNCTIONS *****/
static void __ktest_tc_call_init(void);
static void __ktest_tc_call_clean(void);
static void __ktest_test_case_run_print_pretty(const char* msg, const char* testname);

static void __ktest_tc_call_init(void)
{
    if (ktest_test_case_init != NULL)
        ktest_test_case_init();
}

static void __ktest_tc_call_clean(void)
{
    if (ktest_test_case_clean != NULL)
        ktest_test_case_clean();
}

static void __ktest_test_case_run_print_pretty(const char* msg, const char* testname)
{
    __ktest_print(KTEST_PRINT_INFO("[TEST] %s%.*s")" %s\n",
                  testname,
                  (int)(KTEST_STRING_MAX_LEN - strlen(testname) - strlen("[TEST ] ")),
                  ktest_carray_spaces,
                  msg);
}

/* GETTERS / SETTERS */
ktest_test_result_t __ktest_tc_result_get(void)
{
    return ktest_test_case_result;
}

void __ktest_tc_result_set(ktest_test_result_t res)
{
    ktest_test_case_result = res;
}

int __ktest_get_suite_result(void)
{
    return (int)ktest_tests_failed_counter;
}

/***** GLOBAL FUNCTIONS *****/
void __attribute__(( format(printf,1, 2) )) __ktest_print(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    vfprintf(stderr, fmt, args);
    va_end(args);
}

void __ktest_init(const char* msg, ktest_test_case_init_f tc_init, ktest_test_case_clean_f tc_clean)
{
    if (ktest_framework_state != KTEST_FRAMEWORK_UNINITIALIZED)
    {
        __ktest_print("Please finish prev testsuite before start another\n");
        exit(1);
    }

    /* Init Ktest framework */
    ktest_tests_passed_counter = 0;
    ktest_tests_failed_counter = 0;
    ktest_test_case_init = tc_init;
    ktest_test_case_clean = tc_clean;
    memset(&ktest_carray_spaces, ' ', sizeof(ktest_carray_spaces));
    memset(&ktest_carray_equals, '=', sizeof(ktest_carray_equals));
    srand((unsigned)time(NULL)); /* Some users forgot to set seed */

    /* print ==== msg ===, we need fill spaces, equals char array before this print */
    const int len_with_spaces = (int)strlen(msg) + 2;
    const int left_alignment = (KTEST_STRING_MAX_LEN - len_with_spaces + 1) / 2;
    const int right_alignment = (KTEST_STRING_MAX_LEN - len_with_spaces) / 2;
    __ktest_print(KTEST_PRINT_INFO("%.*s %s %.*s\n"),
                  left_alignment,
                  ktest_carray_equals,
                  msg,
                  right_alignment,
                  ktest_carray_equals);


    /* now we can start*/
    ktest_framework_state = KTEST_FRAMEWORK_STARTED;
}

/* It is easier to pass literal via macro so use macro */
#define KTEST_SUMMARY_TEXT        "TEST SUMMARY"
#define KTEST_SUMMARY_TEXT_LEN    (strlen(KTEST_SUMMARY_TEXT))
#define KTEST_SUMMARY_PRINT_HELPER(COLOR, STR_RESULT) \
    do { \
        const int len_with_spaces = KTEST_SUMMARY_TEXT_LEN + 2; \
        const int left_alignment = (KTEST_STRING_MAX_LEN - len_with_spaces + 1) / 2; \
        const int right_alignment = (KTEST_STRING_MAX_LEN - len_with_spaces) / 2; \
        const ktest_tests_counter_t ktest_tests_executed = ktest_tests_passed_counter + ktest_tests_failed_counter; \
        __ktest_print(COLOR "%.*s" " "KTEST_SUMMARY_TEXT " " "%.*s\n", \
                      left_alignment, \
                      ktest_carray_equals, \
                      right_alignment, \
                      ktest_carray_equals); \
        __ktest_print("TESTS EXECUTED:\t%ld\n", \
                      ktest_tests_executed); \
        __ktest_print("PASSED:\t\t%ld / %ld\n", \
                      ktest_tests_passed_counter, \
                      ktest_tests_executed); \
        __ktest_print("FAILED:\t\t%ld / %ld\n", \
                      ktest_tests_failed_counter, \
                      ktest_tests_executed); \
        __ktest_print(STR_RESULT); \
        __ktest_print("%.*s\n" COLOR_RESET, \
                      KTEST_STRING_MAX_LEN, \
                      ktest_carray_equals); \
    } while(0)

void __ktest_summary(void)
{
    if (ktest_framework_state != KTEST_FRAMEWORK_STARTED)
    {
        __ktest_print("CALL TEST_SUITE_INIT before\n");
        exit(1);
    }

    if (ktest_tests_failed_counter)
        KTEST_SUMMARY_PRINT_HELPER(KTEST_COLOR_FAILED, "TESTS FAILED\n");
    else
        KTEST_SUMMARY_PRINT_HELPER(KTEST_COLOR_PASSED, "TESTS PASSED\n");

    ktest_framework_state = KTEST_FRAMEWORK_UNINITIALIZED;
}

/* We need to create printf fmt from "dynamic" partial format */
const char* __ktest_create_assert_print_fmt(const char* label,
                                            const char* val1_fmt,
                                            const char* op,
                                            const char* val2_fmt)
{
    /* Ktest supports only single thread tests, so static buffer is OK */
    snprintf(&ktest_static_buffer[0],
             sizeof(ktest_static_buffer),
             "[%s] %%s:%%d\t%s %s %s\n",
             label,
             val1_fmt,
             op,
             val2_fmt);
    return &ktest_static_buffer[0];
}

void __ktest_test_case_run_prepare(const char* testname)
{
    if (ktest_framework_state != KTEST_FRAMEWORK_STARTED)
    {
        __ktest_print("CALL TEST_SUITE_INIT before\n");
        exit(1);
    }

    __ktest_tc_call_init();
    __ktest_test_case_run_print_pretty(KTEST_PRINT_INFO("STARTED"), testname);
    __ktest_tc_result_set(KTEST_TEST_RESULT_PASSED);
}

void __ktest_test_case_run_finish(const char* testname)
{
    __ktest_tc_call_clean();
    if (ktest_test_case_result == KTEST_TEST_RESULT_PASSED)
    {
        ++ktest_tests_passed_counter;
        __ktest_test_case_run_print_pretty(KTEST_PRINT_PASSED("PASSED"), testname);
    }
    else
    {
        ++ktest_tests_failed_counter;
        __ktest_test_case_run_print_pretty(KTEST_PRINT_FAILED("FAILED"), testname);
    }
}
