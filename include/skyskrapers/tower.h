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

#ifndef _TOWER_H
#define _TOWER_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _tower tower_t;

extern tower_t *
tower_make(tower_t *in, city_t *parent, int x, int y);

extern tower_t *
tower_copy(tower_t *dst, const tower_t *src);

extern void
tower_free(tower_t *street);

extern int
tower_get_size(const tower_t *tower);

extern bool
tower_set_height(tower_t *tower, int height);

extern int
tower_get_height(const tower_t *tower);

extern bool
tower_is_complete(tower_t *tower);

extern bool
tower_has_floors(const tower_t *tower, int options);

extern bool
tower_and_options(tower_t *tower, int options);

extern int
tower_get_options(tower_t *tower);

/**
 * Вычисление допустимой минимальной высоты здания.
 *
 * @param options Набор битовых флагов допустимых этажей.
 * @param size Максимальная высота зданий.
 *
 * @return Минимальная высота здания, от 1 до @p size.
 */
extern int
tower_get_min_height(const tower_t *tower);

/**
 * Вычисление допустимой максимальной высоты здания.
 *
 * @param options Набор битовых флагов допустимых этажей.
 * @param size Максимальная высота зданий.
 *
 * @return Максимальная высота здания, от 1 до @p size.
 */

extern int
tower_get_max_height(const tower_t *tower);

extern int
tower_get_mask(int bottom, int top);

typedef struct _tower {
    city_t *parent;
    int size;
    int x;
    int y;
    int height;
    int options;
    int weight;
} tower_t;

#ifdef __cplusplus
}
#endif

#endif /* _TOWER_H */
