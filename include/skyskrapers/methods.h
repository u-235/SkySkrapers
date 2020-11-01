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

#ifndef _METHODS_H
#define _METHODS_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _street street_t;
typedef struct _city city_t;

extern bool
method_obvious(const street_t *street);

extern bool
method_exclude(const street_t *street);

/**
 * Ограничивает высоту недостроенных зданий в ряду с подсказкой "2". Высота этих зданий не может
 * быть выше чем максимальная возможная высота первого здания минус один этаж.
 *
 * @param street Проверяемый ряд
 *
 * @return true если были изменения в @p city.
 */
extern bool
method_first_of_two(const street_t *street);

extern bool
method_staircase(const street_t *street);

extern bool
method_step_down(const street_t *street);

extern bool
method_bruteforce(city_t *city);

#ifdef __cplusplus
}
#endif

#endif /* _METHODS_H */
