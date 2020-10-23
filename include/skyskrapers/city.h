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

#ifndef _CITY_H
#define _CITY_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _tower tower_t;

typedef struct _city city_t;

city_t *
city_init(city_t *in, int size);

extern city_t *
city_copy(city_t *dst, const city_t *src);

extern bool
city_is_valid(const city_t *city);

extern bool
city_is_solved(const city_t *city);

extern bool
city_is_deadloop(city_t *city);

extern unsigned long long
city_calc_iteration(const city_t *city);

extern int
city_get_clue(const city_t *city, int side, int pos);

extern tower_t *
city_get_tower(const city_t *city, int side, int pos, int index);

typedef struct _city {
    int size;
    int mask;
    tower_t *towers;
    int *clues;
    bool changed;
    bool must_free;
} city_t;

enum _sides {
    TOP, RIGHT, BOTTOM, LEFT
};

#ifdef __cplusplus
}
#endif

#endif /* _CITY_H */
