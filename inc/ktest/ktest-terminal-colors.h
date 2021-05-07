#ifndef KTEST_TERMINAL_COLORS_H
#define KTEST_TERMINAL_COLORS_H

/*
    This is a private header for ktest.
    Do not include it directly

    Author: Michal Kukowski
    email: michalkukowski10@gmail.com

    LICENCE: GPL3
*/

#ifndef KTEST_FRAMEWORK_H
#error "Never include <ktest/ktest-terminal-colors.h> directly, use <ktest/ktest.h> instead."
#endif

/* ESCAPE COLORS */
#define COLOR_RESET           "\033[0m"
#define COLOR_BLACK           "\033[30m"
#define COLOR_RED             "\033[31m"
#define COLOR_GREEN           "\033[32m"
#define COLOR_YELLOW          "\033[33m"
#define COLOR_BLUE            "\033[34m"
#define COLOR_MAGENTA         "\033[35m"
#define COLOR_CYAN            "\033[36m"
#define COLOR_WHITE           "\033[37m"
#define COLOR_BOLDBLACK       "\033[1m\033[30m"
#define COLOR_BOLDRED         "\033[1m\033[31m"
#define COLOR_BOLDGREEN       "\033[1m\033[32m"
#define COLOR_BOLDYELLOW      "\033[1m\033[33m"
#define COLOR_BOLDBLUE        "\033[1m\033[34m"
#define COLOR_BOLDMAGENTA     "\033[1m\033[35m"
#define COLOR_BOLDCYAN        "\033[1m\033[36m"
#define COLOR_BOLDWHITE       "\033[1m\033[37m"

#endif
