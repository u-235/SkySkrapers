/* utf-8 */

/**
 * @file
 * @brief Решение головоломки SkyScrapers
 * @details
 *
 * @date создан 18.10.2020
 * @author Nick Egorrov
 * @copyright http://www.apache.org/licenses/LICENSE-2.0
 */

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "skyskrapers/skyskrapers.h"
#include "skyskrapers/city.h"
#include "skyskrapers/street.h"
#include "skyskrapers/tower.h"

city_t *
city_make(city_t *in, int size)
{
    city_t *ret;

    if (in == 0) {
        ret = malloc(sizeof(city_t));
        ret->must_free = true;
    } else {
        ret = in;
        ret->must_free = false;
    }

    ret->size = size;
    ret->changed = false;

    int m = 1;

    for (int i = 0; i < size; i++) {
        ret->mask |= m;
        m <<= 1;
    }

    size_t sz = (size_t) size;
    ret->towers = malloc(sz * sz * sizeof(tower_t));

    for (int x = 0; x < size; x++) {
        for (int y = 0; y < size; y++) {
            tower_make(city_get_tower(ret, 0, x, y), ret, x, y);
        }
    }

    ret->need_update = malloc(4 * sz * sizeof(bool));
    ret->need_handle = malloc(4 * sz * sizeof(bool));
    ret->streets = malloc(4 * sz * sizeof(street_t));

    for (int side = 0; side < 4; side ++) {
        for (int pos = 0; pos < size; pos ++) {
            int i = side * size + pos;
            ret->need_update[i] = false;
            ret->need_handle[i] = false;
            street_make(&ret->streets[i], ret, side, pos);
        }
    }

    return ret;
}

city_t *
city_new(int size)
{
    return city_make(0, size);
}

void
city_free(city_t *city)
{
    for (int i = 0; i < 4 * city->size; i ++) {
        street_free(&city->streets[i]);
    }

    free(city->streets);
    free(city->need_update);
    free(city->need_handle);

    for (int i = 0; i < city->size * city->size; i ++) {
        tower_free(&city->towers[i]);
    }

    free(city->towers);

    if (city->must_free) {
        free(city);
    }
}

city_t *
city_copy(city_t *dst, const city_t *src)
{
    city_t *ret = dst;

    if (dst == 0) {
        ret = city_make(0, src->size);
    }

    ret->size = src->size;
    ret->mask = src->mask;
    ret->changed = src->changed;

    for (int i = 0; i < src->size * src->size; i ++) {
        tower_copy(&ret->towers[i], &src->towers[i]);
    }

    for (int i = 0; i < 4 * src->size; i ++) {
        ret->need_update[i] = src->need_update[i];
        ret->need_handle[i] = src->need_handle[i];
        street_copy(&ret->streets[i], &src->streets[i]);
    }

    return ret;
}

void
city_notify_of_tower_change(city_t *city, int x, int y)
{
    assert(city != NULL);
    int sz = city->size;
    assert(x >= 0 && x < city->size);
    assert(y >= 0 && y < city->size);
    /* Side::TOP */
    int i = x;
    city->need_update[i] = true;
    city->need_handle[i] = true;
    /* Side::RIGHT */
    i = sz + y;
    city->need_update[i] = true;
    city->need_handle[i] = true;
    /* Side::BOTTOM */
    i = 3 * sz - x - 1;
    city->need_update[i] = true;
    city->need_handle[i] = true;
    /* Side::LEFT */
    i = 4 * sz - y - 1;
    city->need_update[i] = true;
    city->need_handle[i] = true;
}

void
city_load(city_t *city, int *clues)
{
    for (int side = 0; side < 4; side++) {
        for (int pos = 0; pos < city->size; pos++) {
            int i = side * city->size + pos;
            city->streets[i].clue = clues[i];

            if (clues[i] != 0) {
                city->need_update[i] = true;
                city->need_handle[i] = true;
            }
        }
    }

    for (int side = 0; side < 4; side++) {
        for (int pos = 0; pos < city->size; pos++) {
            int clue = city_get_clue(city, side, pos);

            if (clue == 0) {
                continue;
            }

            tower_t *tower;
            int options = city->mask;

            if (clue == 1) {
                tower = city_get_tower(city, side, pos, 0);
                tower_set_height(tower, city->size);
                options >>= 1;

                for (int i = 1; i < city->size; i++) {
                    tower = city_get_tower(city, side, pos, i);

                    if (tower_has_floors(tower, options)) {
                        tower_and_options(tower, options);
                    }
                }
            } else if (clue == city->size) {
                for (int i = 0; i < city->size; i++) {
                    tower = city_get_tower(city, side, pos, i);
                    tower_set_height(tower, i + 1);
                }
            } else {
                for (int i = city->size; i > 0; i--) {
                    if (i < clue) {
                        options >>= 1;
                    }

                    tower = city_get_tower(city, side, pos, i - 1);

                    if (tower_has_floors(tower, options)) {
                        tower_and_options(tower, options);
                    }
                }
            }
        }
    }
}

typedef struct _view_info {
    int size;
    int highest;
    int visible;
    int foreground;
    int offstage;
    bool valid;
    int options;
} view_info_t;

void
view_info_reset(view_info_t *info, int size)
{
    info->size = size;
    info->highest = 0;
    info->visible = 0;
    info->foreground = 0;
    info->offstage = 0;
    info->valid = true;
    info->options = 0;
}

void
view_info_add(view_info_t *info, int height, int options)
{
    if (options == 0) {
        info->valid = false;
    }

    if (height != 0) {
        info->options ^= options;

        if ((info->options & options) == 0) {
            info->valid = false;
        }
    } else if (info->highest < info->size) {
        if (info->highest == 0) {
            info->foreground++;
        } else {
            info->offstage++;
        }
    }

    if (info->highest < height) {
        info->visible++;
        info->highest = height;
    }
}

bool
city_is_valid(const city_t *city)
{
    for (int side = 0; side < 4; side++) {
        for (int pos = 0; pos < city->size; pos++) {

            view_info_t info;
            view_info_reset(&info, city->size);

            for (int i = 0; i < city->size; i++) {
                tower_t *tower = city_get_tower(city, side, pos, i);
                view_info_add(&info, tower->height, tower->options);

                if (!info.valid) {
                    return false;
                }
            }

            int clue = city_get_clue(city, side, pos);

            if (clue == 0) {
                continue;
            }

            if (info.visible > clue + info.foreground + info.offstage) {
                return false;
            }

            if (info.visible + info.foreground + info.offstage < clue) {
                return false;
            }

            if ((info.visible + info.foreground) == 0 && info.offstage != clue) {
                return false;
            }
        }
    }

    return  true;
}

bool
city_is_solved(const city_t *city)
{
    if (!city_is_valid(city)) {
        return false;
    }

    for (int x = 0; x < city->size; x++) {
        for (int y = 0; y < city->size; y++) {
            if (city_get_tower(city, 0, x, y)->height == 0) {
                return false;
            }
        }
    }

    return true;
}

bool
city_is_deadloop(city_t *city)
{
    bool changes = city->changed;
    city->changed = false;
    return !changes;
}

int
city_get_clue(const city_t *city, int side, int pos)
{
    return city->streets[side * city->size + pos].clue;
}

tower_t *
city_get_tower(const city_t *city, int side, int pos, int index)
{
    int x = 0, y = 0;

    switch (side) {
    case TOP:
        x = pos;
        y = index;
        break;

    case RIGHT:
        x = city->size - 1 - index;
        y = pos;
        break;

    case BOTTOM:
        x = city->size - 1 - pos;
        y = city->size - 1 - index;
        break;

    case LEFT:
        x = index;
        y = city->size - 1 - pos;
        break;

    default:
        fprintf(stderr, "ERROR\nget_tower : bad side=%i\n", side);
        abort();
    }

    return &city->towers[x + y * city->size];
}

int **
city_export(city_t *city)
{
    int **ret = malloc((unsigned int) city->size * sizeof(int *));

    for (int y = 0; y < city->size; y++) {
        int *t = malloc((unsigned int) city->size * sizeof(int));
        ret[y] = t;

        for (int x = 0; x < city->size; x++) {
            tower_t *tower = city_get_tower(city, 0, x, y);
            t[x] = tower->height;
        }
    }

    return ret;
}

unsigned long long
city_calc_iteration(const city_t *city)
{
    unsigned long long result = 1;
    unsigned i = 0;

    for (int x = 0; x < city->size; x++) {
        for (int y = 0; y < city->size; y++) {
            tower_t *tower = city_get_tower(city, 0, x, y);

            if (tower->height == 0) {
                unsigned int v = 0;
                int m = 1;

                for (int h = 0; h < city->size; h++) {
                    if ((tower->options & m) != 0) {
                        v++;
                    }

                    m <<= 1;
                }

                i++;
                result *= v;
            }
        }
    }

    /* Раньше возвращался просто result.
     * Как посчитать реальное значение я не знаю. */
    return result * 2 / i / 3;
}

void
city_print(city_t *city)
{
    FILE *io = stdout;

    fprintf(io, "==============================\n  ");

    for (int x = 0; x < city->size; x++) {
        fprintf(io, "  %3.1i ", city_get_clue(city, 0, x));
    }

    fprintf(io, "\n");

    for (int y = 0; y < city->size; y++) {
        for (int b = city->size; b > 0; b--) {
            if (b == 3) {
                fprintf(io, "%3.1i", city_get_clue(city, 3, city->size - 1 - y));
            } else {
                fprintf(io, "   ");
            }

            for (int x = 0; x < city->size; x++) {
                tower_t *tower = city_get_tower(city, 0, x, y);
                int h = tower->height;
                int o = tower->options;

                if (h == 0) {
                    fprintf(io, " %s ", (o & (1 << (b - 1))) == 0 ? " -- " : " ++ ");
                } else {
                    fprintf(io, " %s ", h >= b ? "####" : "    ");
                }
            }

            if (b == 3) {
                fprintf(io, "%-3.1i", city_get_clue(city, 1, y));
            }

            fprintf(io, "\n");
        }

        if (y < city->size - 1) {
            fprintf(io, "\n");
        }
    }

    fprintf(io, "  ");

    for (int x = 0; x < city->size; x++) {
        fprintf(io, "  %3.1i ", city_get_clue(city, 2, city->size - 1 - x));
    }

    fprintf(io, "\n------------------------------\n\n");
}
