/* utf-8 */

/**
 * @file
 * @brief Решение головоломки SkySkrapers
 * @details
 *
 * @date создан 18.10.2020
 * @author Nick Egorrov
 * @copyright http://www.apache.org/licenses/LICENSE-2.0
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "skyskrapers/skyskrapers.h"
#include "skyskrapers/tower.h"

bool
tower_set_height(tower_t *tower, int height)
{
    if (height < 1 || height > tower->size) {
        fprintf(stderr, "ERROR\ntower_set_height : bad height=%i\n", height);
        abort();
    }

    if (tower->height != 0 && tower->height != height) {
        fprintf(stderr,
                "ERROR\ntower_set_height : height conflict old=%i, new=%i\n",
                tower->height, height);
        abort();
    }

    int old = tower->height;
    tower->height = height;
    tower->options = 1 << (height - 1);
    return old != tower->height;
}

bool
tower_and_options(tower_t *tower, int options)
{
    if (tower->height != 0) {
        fprintf(stdout, "WARNING\ntower_and_options : tower is finished\n");
    }

    if ((tower->options & options) == 0) {
        fprintf(stdout, "ERROR\ntower_and_options : no options\n");
        //abort();
    }

    int old = tower->options;
    tower->options &= options;

    int t = 1;

    for (int h = 1; h <= tower->size; h++) {
        if (t == tower->options) {
            tower->height = h;
            break;
        }

        t <<= 1;
    }

    return old != tower->options;
}

/**
 * Вычисление допустимой минимальной высоты здания.
 *
 * @param tower Указатель на здание.
 *
 * @return Минимальная высота здания, от 1 до @p tower->size.
 */
int
tower_get_min_height(const tower_t *tower)
{
    if (tower->options == 0) {
        return 0;
    }

    int mask = 1;

    for (int i = 1; i <= tower->size; i++) {
        if (tower->options & mask) {
            return i;
        }

        mask <<= 1;
    }

    // ! Error
    return -1;
}

/**
 * Вычисление допустимой максимальной высоты здания.
 *
 * @param tower Указатель на здание.
 *
 * @return Максимальная высота здания, от 1 до @p tower->size.
 */

int
tower_get_max_height(const tower_t *tower)
{
    if (tower->options == 0) {
        return 0;
    }

    int mask = 1 << (tower->size - 1);

    for (int i = tower->size; i > 0; i--) {
        if (tower->options & mask) {
            return i;
        }

        mask >>= 1;
    }

    // ! Error
    return -1;
}

int
tower_get_mask(int bottom, int top)
{
    if (bottom > top) {
        // ! Error
        return 0;
    }

    int mask = 1 << (bottom - 1);

    for (; top > bottom; top--) {
        mask |= mask << 1;
    }

    return mask;
}
