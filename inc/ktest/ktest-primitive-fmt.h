#ifndef KTEST_PRIMITIVE_FMT_H
#define KTEST_PRIMITIVE_FMT_H

/*
    This is a private header for ktest.
    Do not include it directly

    Author: Michal Kukowski
    email: michalkukowski10@gmail.com

    LICENCE: GPL3
*/

#ifndef KTEST_FRAMEWORK_H
#error "Never include <ktest/ktest-primitive-fmt.h> directly, use <ktest/ktest.h> instead."
#endif

/**
 * This macro returning printf fmt for variable x
 */
#define KTEST_PRIMTIVE_GET_FMT(x) \
    _Generic((x), \
             _Bool: "%u", \
             char: "%c", \
             signed char: "%c", \
             unsigned char: "%c", \
             short: "%hd", \
             unsigned short: "%hu", \
             int: "%d", \
             unsigned int: "%u", \
             long: "%ld", \
             unsigned long: "%lu", \
             long long: "%lld", \
             unsigned long long: "%llu", \
             float: "%f", \
             double: "%lf", \
             long double: "%Lf", \
             default: "%p" \
    )

typedef enum KTEST_PRIMITIVES
{
    KTEST_PRIMITIVES_BOOL,
    KTEST_PRIMITIVES_CHAR,
    KTEST_PRIMITIVES_SIGNED_CHAR,
    KTEST_PRIMITIVES_UNSIGNED_CHAR,
    KTEST_PRIMITIVES_SHORT,
    KTEST_PRIMITIVES_UNSIGNED_SHORT,
    KTEST_PRIMITIVES_INT,
    KTEST_PRIMITIVES_UNSIGNED_INT,
    KTEST_PRIMITIVES_LONG,
    KTEST_PRIMITIVES_UNSIGNED_LONG,
    KTEST_PRIMITIVES_LONG_LONG,
    KTEST_PRIMITIVES_UNSIGNED_LONG_LONG,
    KTEST_PRIMITIVES_FLOAT,
    KTEST_PRIMITIVES_DOUBLE,
    KTEST_PRIMITIVES_LONG_DOUBLE,
    KTEST_PRIMITIVES_NON_PRIMITIVE
} KTEST_PRIMITIVES;

#define KTEST_PRIMITIVE_GET_TYPE(x) \
    _Generic((x), \
             _Bool: KTEST_PRIMITIVES_BOOL, \
             char: KTEST_PRIMITIVES_CHAR, \
             signed char: KTEST_PRIMITIVES_SIGNED_CHAR, \
             unsigned char: KTEST_PRIMITIVES_UNSIGNED_CHAR, \
             short: KTEST_PRIMITIVES_SHORT, \
             unsigned short: KTEST_PRIMITIVES_UNSIGNED_SHORT, \
             int: KTEST_PRIMITIVES_INT, \
             unsigned int: KTEST_PRIMITIVES_UNSIGNED_INT, \
             long: KTEST_PRIMITIVES_LONG, \
             unsigned long: KTEST_PRIMITIVES_UNSIGNED_LONG, \
             long long: KTEST_PRIMITIVES_LONG_LONG, \
             unsigned long long: KTEST_PRIMITIVES_UNSIGNED_LONG_LONG, \
             float: KTEST_PRIMITIVES_FLOAT, \
             double: KTEST_PRIMITIVES_DOUBLE, \
             long double: KTEST_PRIMITIVES_LONG_DOUBLE, \
             default: KTEST_PRIMITIVES_NON_PRIMITIVE \
    )

#define KTEST_PRIMITIVES_PROBABLY_POINTER(x) (KTEST_PRIMITIVE_GET_TYPE(x) == KTEST_PRIMITIVES_NON_PRIMITIVE && sizeof(x) == sizeof(void *))

#endif
