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

#include <assert.h>
#include <stddef.h>
#include "skyskrapers/city.h"
#include "skyskrapers/tower.h"

extern tower_t *
tower_init(tower_t *in, city_t *parent, int x, int y)
{
    assert(parent != NULL);
    assert(in != NULL);
    assert(x >= 0 && x < parent->size);
    assert(y >= 0 && y < parent->size);
    tower_t *ret = in;
    ret->parent = parent;
    ret->size = parent->size;
    ret->x = x;
    ret->y = y;
    ret->height = 0;
    ret->options = parent->mask;
    ret->weight = 0;

    return ret;
}

tower_t *
tower_copy(tower_t *dst, const tower_t *src)
{
    assert(dst != NULL);
    tower_t *ret = dst;
    assert(src != NULL);
    assert(src->size == ret->size);

    if (ret == src) {
        return ret;
    }

    ret->parent = src->parent;
    ret->x = src->x;
    ret->y = src->y;
    ret->height = src->height;
    ret->options = src->options;
    ret->weight = src->weight;

    return ret;
}

void
tower_free(tower_t *tower)
{
    assert(tower != NULL);
}

int
tower_get_size(const tower_t *tower)
{
    assert(tower != NULL);
    return tower->size;
}

bool
tower_set_height(tower_t *tower, int height)
{
    assert(tower != NULL);
    assert(height > 0 && height <= tower->size);
    assert(tower->height == 0 || tower->height == height);
    int old = tower->height;
    tower->height = height;
    tower->options = 1 << (height - 1);
    return old != tower->height;
}

extern int
tower_get_height(const tower_t *tower)
{
    assert(tower != NULL);
    return tower->height;
}

bool
tower_is_complete(tower_t *tower)
{
    assert(tower != NULL);
    return tower_get_height(tower) != 0;
}

extern bool
tower_has_floors(const tower_t *tower, int options)
{
    assert(tower != NULL);
    return (tower->options & options) != 0;
}

bool
tower_and_options(tower_t *tower, int options)
{
    assert(tower != NULL);
    assert(tower_has_floors(tower, options));
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

int
tower_get_options(tower_t *tower)
{
    assert(tower != NULL);
    return tower->options;
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
    assert(tower != NULL);

    if (tower->options == 0) {
        return 0;
    }

    int mask = 1;
    int ret = 1;

    while (ret <= tower->size && (tower->options & mask) == 0) {
        ret++;
        mask <<= 1;
    }

    return ret;
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
    assert(tower != NULL);

    if (tower->options == 0) {
        return 0;
    }

    int mask = 1 << (tower->size - 1);
    int ret = tower->size;

    while (ret > 0 && (tower->options & mask) == 0) {
        ret--;
        mask >>= 1;
    }

    return ret;
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
