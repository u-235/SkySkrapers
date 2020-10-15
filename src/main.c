/* utf-8 */

/**
 * @file
 * @brief Решение головоломки SkySkrapers
 * @details
 *
 * @date создан 25.09.2020
 * @author Nick Egorrov
 * @copyright http://www.apache.org/licenses/LICENSE-2.0
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "skyskrapers.h"

#define MAX_PUZZLE 8u

int **
SolvePuzzle(int size, int *clues)
{
    city_t *city = city_new(size);
    city_load(city, clues);
    fprintf(stdout, "\n  Load puzzle\n");
    city_print(city);
    city_solve(city);
    int **result = city_export(city);
    city_free(city);
    return result;
}

struct _test {
    char *title;
    int result;
    int size;
    int clues[4 * MAX_PUZZLE];
    int expected[MAX_PUZZLE][MAX_PUZZLE];
} tests[] = {
    {
        "4x4 1", 0, 4,
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
        "4x4 2", 0, 4,
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
        "5x5 light 1", 0, 5,
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
        "5x5 light 2", 0, 5,
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
        "5x5 middle", 0, 5,
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
        "6x6 middle", 0, 6,
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
        "6x6 hard", 0, 6,
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
main()
{
    for (size_t i = 0; i < sizeof(tests) / sizeof(struct _test); i++) {
        tests[i].result = equal(tests[i].size,
                                SolvePuzzle(tests[i].size, tests[i].clues),
                                tests[i].expected);
        fprintf(stdout, "Test \"%s\" : %s\n\n",
                tests[i].title,
                tests[i].result > 0 ? "OK" : tests[i].result == 0 ? "UNCOMPLETE" : "FAIL");
    }

    fprintf(stdout, "Tests result\n");

    for (size_t i = 0; i < sizeof(tests) / sizeof(struct _test); i++) {
        fprintf(stdout, "%2.1i %-16.16s : %s\n",
                i + 1,
                tests[i].title,
                tests[i].result > 0 ? "OK" : tests[i].result == 0 ? "UNCOMPLETE" : "FAIL");
    }
}
