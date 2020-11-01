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

#ifndef _CITY_H
#define _CITY_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _tower tower_t;

typedef struct _street street_t;

typedef struct _city city_t;

city_t *
city_make(city_t *in, int size);

extern city_t *
city_copy(city_t *dst, const city_t *src);

extern void
city_notify_of_tower_change(city_t *city, int x, int y);

extern bool
city_is_valid(const city_t *city);

extern bool
city_is_solved(const city_t *city);

extern unsigned long long
city_calc_iteration(const city_t *city);

extern int
city_get_clue(const city_t *city, int side, int pos);

extern tower_t *
city_get_tower(const city_t *city, int side, int pos, int index);

/**
 * Represents a puzzle.
 */
typedef struct _city {
    /** Size of puzzle. This field is constant. */
    int size;
    /** Mask for all floors. */
    int mask;
    /** Array of street_t.
     *
     * Size is city_t::size ^ 2.
     */
    tower_t *towers;

    /** Array of street_t.
     *
     * Size is 4 times city_t::size.
     */
    street_t *streets;
    /**
     * Array of flags indicating the need to update the corresponding instance from
     * city_t::streets.
     *
     * Size is 4 times city_t::size.
     */
    bool *need_update;
    /**
     * Array of flags indicating the need to process the corresponding instance from
     * city_t::streets.
     *
     * Size is 4 times city_t::size.
     */
    bool *need_handle;

    bool must_free;
} city_t;

enum _sides {
    TOP, RIGHT, BOTTOM, LEFT
};

#ifdef __cplusplus
}
#endif

#endif /* _CITY_H */
