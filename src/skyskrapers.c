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

bool
city_is_valid(city_t *city);

bool
city_is_solved(city_t *city);

bool
city_is_deadloop(city_t *city);

bool
city_do_obvious_highest(city_t *city);

bool
city_do_exclude(city_t *city);

bool
city_do_first_of_many(city_t *city);

enum _sides {
    TOP, RIGHT, BOTTOM, LEFT
};

typedef struct _tower {
    int size;
    int height;
    int options;
} tower_t;

typedef struct _city {
    int size;
    int mask;
    tower_t *towers;
    int *clues;
    bool changed;
    bool must_free;
} city_t;

bool
city_solve(city_t *city)
{
    for (int pass = 1; !city_is_solved(city); pass++) {
        if (!city_is_valid(city)) {
            fprintf(stdout, "ERROR\nInvalid city.\n");
            return false;
        }

        fprintf(stdout, "Pass %i\n", pass);

        if (city_do_obvious_highest(city)) {
            fprintf(stdout, "Obvious highest\n");
            city_print(city);
        }

        if (city_do_exclude(city)) {
            fprintf(stdout, "Exclude\n");
            city_print(city);
        }

        if (city_do_first_of_many(city)) {
            fprintf(stdout, "First of two\n");
            city_print(city);
        }

        if (city_is_deadloop(city)) {
            return false;
        }
    }

    return true;
}

int
city_get_clue(city_t *city, int side, int pos)
{
    return city->clues[side * city->size + pos];
}

tower_t *
city_get_tower(city_t *city, int side, int pos, int index)
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

city_t *
city_init(city_t *in, int size)
{
    city_t *ret;

    if (in == 0) {
        ret = malloc(sizeof(city_t));
        ret->must_free = true;
    } else {
        ret = in;
        ret->must_free = false;
    }

    ret->clues = 0;
    ret->size = size;
    ret->changed = false;
    ret->towers = malloc(size * size * sizeof(tower_t));

    int m = 1;

    for (int i = 0; i < size; i++) {
        ret->mask |= m;
        m <<= 1;
    }

    for (int x = 0; x < size; x++) {
        for (int y = 0; y < size; y++) {
            tower_t *tower = city_get_tower(ret, 0, x, y);
            tower->size = size;
            tower->height = 0;
            tower->options = ret->mask;
        }
    }

    return ret;
}

city_t *
city_new(int size)
{
    return city_init(0, size);
}

void
city_free(city_t *city)
{
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
        ret = city_init(0, src->size);
    }

    dst->size = src->size;
    dst->mask = src->mask;
    dst->clues = src->clues;
    dst->changed = src->changed;
    memcpy(dst->towers, src->towers, src->size * src->size * sizeof(tower_t));

    return ret;
}

void
city_load(city_t *city, int *clues)
{
    city->clues = clues;

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
                    tower_and_options(tower, options);
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
                    tower_and_options(tower, options);
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
city_is_valid(city_t *city)
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

            if (info.visible > clue) {
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
city_is_solved(city_t *city)
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

bool
city_do_obvious_highest(city_t *city)
{
    bool changed = false;
    int options = 1 << city->size;

    for (int h = city->size; h > 0; h--) {
        options >>= 1;

        for (int side = 0; side <= 1; side++) {
            for (int pos = 0; pos < city->size; pos++) {
                tower_t *highest = 0;

                for (int i = 0; i < city->size; i++) {
                    tower_t *tower = city_get_tower(city, side, pos, i);

                    if (tower->height == h) {
                        highest = 0;
                        break;
                    }

                    if (tower->options & options) {
                        if (highest == 0) {
                            highest = tower;
                        } else {
                            highest = 0;
                            break;
                        }
                    }
                }

                if (highest != 0) {
                    tower_set_height(highest, h);
                    changed = true;
                    city->changed = true;
                    //return true;
                }
            }
        }
    }

    return  changed;
}

bool
city_do_exclude(city_t *city)
{
    bool changed = false;

    for (int side = 0; side <= 1; side++) {
        for (int pos = 0; pos < city->size; pos++) {
            int options = city->mask;

            for (int i = 0; i < city->size; i++) {
                tower_t *tower = city_get_tower(city, side, pos, i);

                if (tower->height != 0) {
                    options &= ~tower->options;
                }
            }

            for (int i = 0; i < city->size; i++) {
                tower_t *tower = city_get_tower(city, side, pos, i);

                if (tower->height == 0) {
                    if (tower_and_options(tower, options)) {
                        changed = true;
                        city->changed = true;
                        //return true;
                    }
                }
            }
        }
    }

    return  changed;
}

bool
city_do_first_of_many(city_t *city)
{
    bool changed = false;

    for (int side = 0; side < 4; side++) {
        for (int pos = 0; pos < city->size; pos++) {
            int clue = city_get_clue(city, side, pos);

            if (clue < 2 || city_get_tower(city, side, pos, 0)->height != 0) {
                continue;
            }

            for (int i = 0; i < city->size; i++) {
                tower_t *tower = city_get_tower(city, side, pos, i);

                if (tower->options & (1 << (city->size - 1))) {
                    if (tower_and_options(city_get_tower(city, side, pos, 0),
                                          city->mask & (city->mask << (i + 1 - clue)))) {
                        city->changed = true;
                        changed = true;
                    }

                    break;
                }
            }
        }
    }

    return  changed;
}

int **
city_export(city_t *city)
{
    int **ret = malloc(sizeof(int *) * city->size);

    for (int y = 0; y < city->size; y++) {
        int *t = malloc(sizeof(int) * city->size);
        ret[y] = t;

        for (int x = 0; x < city->size; x++) {
            tower_t *tower = city_get_tower(city, 0, x, y);
            t[x] = tower->height;
        }
    }

    return ret;
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
    fflush(io);
}
