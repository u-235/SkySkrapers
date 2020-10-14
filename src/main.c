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
    int result;
    int size;
    int clues[4 * MAX_PUZZLE];
    int expected[MAX_PUZZLE][MAX_PUZZLE];
} tests[] = {
    {/* test 4x4 №1 */
        0, 4,
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
    {/* test 4x4 №2 */
        0, 4,
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
    {/* test 6x6 №1 */
        0, 6,
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
    {/* test 6x6 №2 */
        0, 6,
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
    }
};

int
equal(int size, int **puzzle, int expected[MAX_PUZZLE][MAX_PUZZLE])
{
    if (!puzzle || !expected) {
        return 0;
    }

    for (int i = 0; i < size; ++i) {
        if (memcmp(puzzle[i], expected[i], size * sizeof(int))) {
            return 0;
        }
    }

    return 1;
}

int
main()
{
    for (size_t i = 0; i < sizeof(tests) / sizeof(struct _test); i++) {
        tests[i].result = equal(tests[i].size,
                                SolvePuzzle(tests[i].size, tests[i].clues),
                                tests[i].expected);
        fprintf(stdout, "Test %i : %s\n\n", i + 1, tests[i].result ? "BINGO!!" : "FAIL((");
    }
}
