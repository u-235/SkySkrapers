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
SolvePuzzle(size_t size, int *clues)
{
    (void) size;
    city_t *city = city_new();
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
    size_t size;
    int clues[4 * MAX_PUZZLE];
    int expected[MAX_PUZZLE][4];
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
    }
};

int
equal(size_t size, int **puzzle, int expected[][4])
{
    if (!puzzle || !expected) {
        return 0;
    }

    for (int i = 0; i < 4; ++i) {
        if (memcmp(puzzle[i], expected[i], size * sizeof(int))) {
            return 0;
        }
    }

    return 1;
}

int
main()
{
    for (size_t i = 0; i < 2; i++) {
        tests[i].result = equal(tests[i].size,
                                SolvePuzzle(tests[i].size, tests[i].clues),
                                tests[i].expected);
        fprintf(stdout, "Test result : %s\n\n", tests[i].result ? "BINGO!!" : "FAIL((");
    }
}
