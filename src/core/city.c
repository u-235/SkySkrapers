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
    ret->mask = tower_get_mask(1, size);
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
    assert(city != NULL);

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
    assert(src != NULL);
    city_t *ret = dst;

    if (dst == 0) {
        ret = city_make(0, src->size);
    }

    ret->size = src->size;
    ret->mask = src->mask;

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
    assert(x >= 0 && x < sz);
    assert(y >= 0 && y < sz);
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
city_notify_of_street_change(city_t *city, int side, int pos)
{
    assert(city != NULL);
    assert(side >= 0 && side < 4);
    assert(pos >= 0 && pos < city->size);
    int i = side * city->size + pos;
    city->need_update[i] = true;
    city->need_handle[i] = true;

}

static void
load_clues(city_t *city, const int *clues, bool constraint)
{
    for (int i = 0; i < 4 * city->size; i++) {
        street_t *street = &city->streets[i];
        int clue = clues[i];
        street_set_clue(street, clue);

        if (constraint) {
            street_fast_constraint(street);
        }
    }
}

void
city_load_clues(city_t *city, const int *clues)
{
    assert(city != NULL);
    assert(clues != NULL);
    load_clues(city, clues, true);
}

void
city_set_clues(city_t *city, const int *clues)
{
    assert(city != NULL);
    assert(clues != NULL);
    load_clues(city, clues, false);
}

bool
city_is_valid(const city_t *city)
{
    assert(city != NULL);
    bool valid = true;

    for (int i = 0; i < 4 * city->size; i ++) {
        street_t *street = &city->streets[i];

        if (city->need_update[i]) {
            street_update(street);
            city->need_update[i] = false;
        }

        if (!street->valid) {
            valid = false;
        }
    }

    return  valid;
}

bool
city_is_solved(const city_t *city)
{
    assert(city != NULL);

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

static int
city_get_clue(const city_t *city, int side, int pos)
{
    assert(city != NULL);
    return city->streets[side * city->size + pos].clue;
}

tower_t *
city_get_tower(const city_t *city, int side, int pos, int index)
{
    assert(city != NULL);
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

/**
 * Возвращает динамический двухмерный массив с высотами башен. Если башня недостроена, то высота
 * равна нулю. Если массив больше не нужен, то он должен быть удален функцией free().
 *
 * @param [in] city Объект для копирования высот в массив.
 * @return Динамический двухмерный массив с высотами башен.
 */
int **
city_get_heights(const city_t *city)
{
    assert(city != NULL);
    unsigned int sz = (unsigned int) city->size;
    int **ret = malloc(sz * sizeof(int *) + sz * sz * sizeof(int));
    assert(ret != NULL);

    for (int y = 0; y < city->size; y++) {
        int *t = (int *) &ret[sz] + (unsigned int) y * sz;
        ret[y] = t;

        for (int x = 0; x < city->size; x++) {
            tower_t *tower = city_get_tower(city, 0, x, y);
            t[x] = tower_get_height(tower);
        }
    }

    return ret;
}

/**
 * Устанавливает высоты башен из динамического двухмерного массива.
 *
 * @param [in] city Объект для установки высот.
 * @param [in] heights Динамический двухмерный массив с высотами башен.
 */
void
city_set_heights(city_t *city, const int **heights)
{
    assert(city != NULL);
    assert(heights != NULL);

    for (int y = 0; y < city->size; y++) {
        for (int x = 0; x < city->size; x++) {
            tower_set_height(city_get_tower(city, 0, x, y), heights[y][x]);
        }
    }
}

/**
 * Возвращает динамический двухмерный массив массив с битовыми флагами этажей башен. Если массив
 * больше не нужен, то он должен быть удален функцией free().
 *
 * @param [in] city Объект для копирования этажей в массив.
 * @return Динамический двухмерный массив с этажами башен.
 */
int **
city_get_floors(const city_t *city)
{
    assert(city != NULL);
    unsigned int sz = (unsigned int) city->size;
    int **ret = malloc(sz * sizeof(int *) + sz * sz * sizeof(int));
    assert(ret != NULL);

    for (int y = 0; y < city->size; y++) {
        int *t = (int *) &ret[sz] + (unsigned int) y * sz;
        ret[y] = t;

        for (int x = 0; x < city->size; x++) {
            tower_t *tower = city_get_tower(city, 0, x, y);
            t[x] = tower_get_options(tower);
        }
    }

    return ret;

}

/**
 * Устанавливает этажи башен из динамического двухмерного массива.
 *
 * @param [in] city Объект для установки этажей.
 * @param [in] floors Динамический двухмерный массив с этажами башен.
 */
void
city_set_floors(city_t *city, const int **floors)
{
    assert(city != NULL);
    assert(floors != NULL);

    for (int y = 0; y < city->size; y++) {
        for (int x = 0; x < city->size; x++) {
            tower_set_options(city_get_tower(city, 0, x, y), floors[y][x]);
        }
    }

}

unsigned long long
city_calc_iteration(const city_t *city)
{
    assert(city != NULL);
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
city_print(const city_t *city)
{
    assert(city != NULL);
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
