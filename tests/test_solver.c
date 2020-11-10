/* utf-8 */

/**
 * @file
 * @brief Тесты головоломки SkyScrapers
 * @details
 *
 * @date создан 25.09.2020
 * @author Nick Egorrov
 * @copyright http://www.apache.org/licenses/LICENSE-2.0
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <criterion/criterion.h>

#include "skyskrapers/skyskrapers.h"

#define MAX_PUZZLE 8u

struct _test {
    char *title;
    clock_t clock;
    int result;
    int size;
    int clues[4 * MAX_PUZZLE];
    int expected[MAX_PUZZLE][MAX_PUZZLE];
} tests[] = {
    {
        "4x4 1", 0, 0, 4,
        { /* clues */
            2, 2, 1, 3,
            2, 2, 3, 1,
            1, 2, 2, 3,
            3, 2, 1, 3
        }, {  /* expected */
            { 1, 3, 4, 2 },
            { 4, 2, 1, 3 },
            { 3, 4, 2, 1 },
            { 2, 1, 3, 4 }
        }
    },
    {
        "4x4 2", 0, 0, 4,
        {  /* clues */
            0, 0, 1, 2,
            0, 2, 0, 0,
            0, 3, 0, 0,
            0, 1, 0, 0
        }, {  /* expected */
            { 2, 1, 4, 3 },
            { 3, 4, 1, 2 },
            { 4, 2, 3, 1 },
            { 1, 3, 2, 4 }
        }
    },
    {
        "5x5 light 1", 0, 0, 5,
        {  /* clues */
            5, 0, 3, 1, 0,
            0, 1, 0, 0, 5,
            3, 0, 0, 0, 0,
            0, 2, 0, 0, 0
        }, {  /* expected */
            { 1, 3, 2, 5, 4 },
            { 2, 1, 4, 3, 5 },
            { 3, 5, 1, 4, 2 },
            { 4, 2, 5, 1, 3 },
            { 5, 4, 3, 2, 1 }
        }
    },
    {
        "5x5 light 2", 0, 0, 5,
        { /* clues */
            0, 4, 0, 0, 0,
            0, 0, 5, 1, 0,
            0, 3, 5, 0, 3,
            0, 3, 0, 4, 0
        }, {  /* expected */
            { 4, 2, 5, 1, 3 },
            { 1, 3, 4, 5, 2 },
            { 5, 4, 3, 2, 1 },
            { 3, 1, 2, 4, 5 },
            { 2, 5, 1, 3, 4 }
        }
    },
    {
        "5x5 middle", 0, 0, 5,
        { /* clues */
            0, 5, 0, 0, 0,
            0, 0, 2, 0, 0,
            0, 4, 0, 0, 3,
            0, 0, 0, 3, 0
        }, {  /* expected */
            { 4, 1, 3, 5, 2 },
            { 3, 2, 1, 4, 5 },
            { 5, 3, 2, 1, 4 },
            { 2, 4, 5, 3, 1 },
            { 1, 5, 4, 2, 3 }
        }
    },
    {
        "6x6 middle", 0, 0, 6,
        {  /* clues */
            0, 0, 0, 2, 2, 0,
            0, 0, 0, 6, 3, 0,
            0, 4, 0, 0, 0, 0,
            4, 4, 0, 3, 0, 0
        }, {  /* expected */
            { 5, 6, 1, 4, 3, 2 },
            { 4, 1, 3, 2, 6, 5 },
            { 2, 3, 6, 1, 5, 4 },
            { 6, 5, 4, 3, 2, 1 },
            { 1, 2, 5, 6, 4, 3 },
            { 3, 4, 2, 5, 1, 6 }
        }
    },
    {
        "6x6 hard", 0, 0, 6,
        {  /* clues */
            3, 2, 2, 3, 2, 1,
            1, 2, 3, 3, 2, 2,
            5, 1, 2, 2, 4, 3,
            3, 2, 1, 2, 2, 4
        }, {  /* expected */
            { 2, 1, 4, 3, 5, 6 },
            { 1, 6, 3, 2, 4, 5 },
            { 4, 3, 6, 5, 1, 2 },
            { 6, 5, 2, 1, 3, 4 },
            { 5, 4, 1, 6, 2, 3 },
            { 3, 2, 5, 4, 6, 1 }
        }
    },
    {
        "7x7 light", 0, 0, 7,
        {  /* clues */
            0, 2, 3, 0, 2, 0, 0,
            5, 0, 4, 5, 0, 4, 0,
            0, 4, 2, 0, 0, 0, 6,
            5, 2, 2, 2, 2, 4, 1
        }, {  /* expected */
            { 7, 6, 2, 1, 5, 4, 3 },
            { 1, 3, 5, 4, 2, 7, 6 },
            { 6, 5, 4, 7, 3, 2, 1 },
            { 5, 1, 7, 6, 4, 3, 2 },
            { 4, 2, 1, 3, 7, 6, 5 },
            { 3, 7, 6, 2, 1, 5, 4 },
            { 2, 4, 3, 5, 6, 1, 7 }
        }
    },
    {
        "7x7 hard", 0, 0, 7,
        {  /* clues */
            7, 0, 0, 0, 2, 2, 3,
            0, 0, 3, 0, 0, 0, 0,
            3, 0, 3, 0, 0, 5, 0,
            0, 0, 0, 0, 5, 0, 4
        }, {  /* expected */
            { 1, 5, 6, 7, 4, 3, 2 },
            { 2, 7, 4, 5, 3, 1, 6 },
            { 3, 4, 5, 6, 7, 2, 1 },
            { 4, 6, 3, 1, 2, 7, 5 },
            { 5, 3, 1, 2, 6, 4, 7 },
            { 6, 2, 7, 3, 1, 5, 4 },
            { 7, 1, 2, 4, 5, 6, 3 }
        }
    },
    {
        "7x7 medved", 0, 0, 7,
        {  /* clues */
            3, 3, 2, 1, 2, 2, 3,
            4, 3, 2, 4, 1, 4, 2,
            2, 4, 1, 4, 5, 3, 2,
            3, 1, 4, 2, 5, 2, 3
        }, {  /* expected */
            {2, 1, 4, 7, 6, 5, 3},
            {6, 4, 7, 3, 5, 1, 2},
            {1, 2, 3, 6, 4, 7, 5},
            {5, 7, 6, 2, 3, 4, 1},
            {4, 3, 5, 1, 2, 6, 7},
            {7, 6, 2, 5, 1, 3, 4},
            {3, 5, 1, 4, 7, 2, 6}
        }
    }
};

int
equal(int size, int **puzzle, int expected[MAX_PUZZLE][MAX_PUZZLE])
{
    if (!puzzle || !expected) {
        return -1;
    }

    int ret = 1;

    for (int i = 0; i < size; ++i) {
        int *p = puzzle[i];
        int *e = expected[i];

        for (int j = 0; j < size; j++) {
            if (p[j] == 0) {
                ret = 0;
            } else if (p[j] != e[j]) {
                return -2;
            }
        }
    }

    return ret;
}

int
do_test(struct _test t)
{
    city_t *city = city_new(t.size);
    city_load_clues(city, t.clues);
    fprintf(stdout, "\nLoad puzzle %s\n", t.title);
    city_print(city);
    city_solve(city);
    city_print(city);
    int **rows = city_get_heights(city);
    city_free(city);
    int result = equal(t.size, rows, t.expected);
    free(rows);
    return result;
}

Test(TestSolver, TestAll)
{
    for (size_t i = 0; i < sizeof(tests) / sizeof(struct _test); i++) {
        tests[i].clock = clock();
        int result = do_test(tests[i]);
        tests[i].clock = clock() - tests[i].clock;
        tests[i].result = result;
        fflush(stdout);
        cr_expect(result != 0, "Puzzle not completed.");
        cr_expect(result >= 0, "Puzzle solution failed.");
    }

    fflush(stdout);
    fprintf(stdout, "Tests result\n");

    for (size_t i = 0; i < sizeof(tests) / sizeof(struct _test); i++) {
        fprintf(stdout, "%2.1i %-16.16s [ %-11.11s] %7.3f ms\n",
                i + 1,
                tests[i].title,
                tests[i].result > 0 ? "OK" : tests[i].result == 0 ? "UNCOMPLETE" : "FAIL",
                (tests[i].clock * 1000.0) / CLOCKS_PER_SEC);
    }
}
