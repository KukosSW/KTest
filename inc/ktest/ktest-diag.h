#ifndef KTEST_DIAG_H
#define KTEST_DIAG_H

/*
    This is a private header for ktest.
    Do not include it directly

    Author: Michal Kukowski
    email: michalkukowski10@gmail.com

    LICENCE: GPL3
*/

#ifndef KTEST_FRAMEWORK_H
#error "Never include <ktest/ktest-diag.h> directly, use <ktest/ktest.h> instead."
#endif

#ifndef TOSTRING
#define KTEST_TOSTRING(x) #x
#define TOSTRING(x) KTEST_TOSTRING(x)
#endif

#ifdef __clang__

#define KTEST_DIAG_CLANG(msg)     _Pragma(TOSTRING(clang diagnostic msg))
#define KTEST_DIAG_PUSH()         KTEST_DIAG_CLANG(push)
#define KTEST_DIAG_POP()          KTEST_DIAG_CLANG(pop)
#define KTEST_DIAG_WARNING(str)   KTEST_DIAG_CLANG(warning str)
#define KTEST_DIAG_ERROR(str)     KTEST_DIAG_CLANG(error str)
#define KTEST_DIAG_IGNORE(str)    KTEST_DIAG_CLANG(ignored str)

#elif defined(__GNUC__)

#define KTEST_DIAG_GCC(msg)       _Pragma(TOSTRING(GCC diagnostic msg))
#define KTEST_DIAG_PUSH()         KTEST_DIAG_GCC(push)
#define KTEST_DIAG_POP()          KTEST_DIAG_GCC(pop)
#define KTEST_DIAG_WARNING(str)   KTEST_DIAG_GCC(warning str)
#define KTEST_DIAG_ERROR(str)     KTEST_DIAG_GCC(error str)
#define KTEST_DIAG_IGNORE(str)    KTEST_DIAG_GCC(ignored str)

#else

#define KTEST_DIAG_PUSH()
#define KTEST_DIAG_POP()
#define KTEST_DIAG_WARNING(str)
#define KTEST_DIAG_ERROR(str)
#define KTEST_DIAG_IGNORE(str)

#endif

#endif
