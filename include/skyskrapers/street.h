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

#ifndef _STREET_H
#define _STREET_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _city city_t;
typedef struct _street street_t;
typedef struct _tower tower_t;

extern street_t *
street_make(street_t *in, city_t *parent, int side, int pos);

extern street_t *
street_copy(street_t *dst, const street_t *src);

extern void
street_free(street_t *street);

extern void
street_set_clue(street_t *street, int clue);

extern int
street_get_clue(const street_t *street);

extern tower_t *
street_get_tower(const street_t *street, int index);

extern void
street_fast_constraint(street_t *street);

extern bool
street_update(street_t *street);

typedef struct _hill {
    int first;
    int last;
    int vacant;
    int action_mask;
    int shadow;
    int top;
    int bottom;
} hill_t;

typedef struct _street {
    city_t *parent;
    int size;
    tower_t **towers;
    int side;
    int pos;
    int clue;
    bool valid;
    /** Индекс первого самого высокого здания. */
    int highest_first;
    /** Индекс последнего самого высокого здания. */
    int highest_last;
    /** Количество однозначно видимых построенных зданий на улице.*/
    int visible;
    /** Общее количество строящихся зданий, которые могут повлиять на видимость.*/
    int vacant;
    /** Количество фрагментов рельефа. Каждый фрагмент начинается с недостроенного
     * видимого здания и заканчиваетя недостроенным зданием, за которым следует
     * построенное здание выше максимальной высоты фрагмента.*/
    int hill_count;
    hill_t *hill_array;
} street_t;

#ifdef __cplusplus
}
#endif

#endif /* _STREET_H */
